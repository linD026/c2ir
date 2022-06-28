#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/IRPrintingPasses.h>

#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

#include <llvm/Bitstream/BitstreamReader.h>
#include <llvm/Bitstream/BitstreamWriter.h>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>

#include <stack>
#include <vector>
#include <memory>
#include <string>
#include <map>

#include "ASTnode.hpp"
#include "parser.hpp"

using namespace llvm;
using legacy::PassManager;
using std::unique_ptr;
using std::string;

class CodeGenBlock {
public:
    BasicBlock *block;
    Value *returnValue;
    std::map<string, Value *> locals;
};

class CodeGenContext {
    std::vector<CodeGenBlock *> blockStack;

public:
    LLVMContext llvmContext;
    IRBuilder<> builder;
    Function *mainFunction;
    unique_ptr<Module> module;

    CodeGenContext()
        : builder(llvmContext)
    {
        module = unique_ptr<Module>(new Module("main", this->llvmContext));
    }

    std::map<std::string, Value *> &locals()
    {
        return blockStack.front()->locals;
    }

    BasicBlock *currentBlock() const
    {
        return blockStack.back()->block;
    }

    void pushBlock(BasicBlock *block)
    {
        CodeGenBlock *codeGenBlock = new CodeGenBlock();
        codeGenBlock->block = block;
        codeGenBlock->returnValue = nullptr;
        blockStack.push_back(codeGenBlock);
    }

    void popBlock()
    {
        CodeGenBlock *codeGenBlock = blockStack.back();
        blockStack.pop_back();
        delete codeGenBlock;
    }

    void generateCode(NBlock &root)
    {
        cout << "Generating IR code" << endl;

        std::vector<Type *> sysArgs;
        FunctionType *mainFuncType = FunctionType::get(
            Type::getVoidTy(this->llvmContext), makeArrayRef(sysArgs), false);
        mainFunction = Function::Create(mainFuncType,
                                        GlobalValue::ExternalLinkage, "main");
        BasicBlock *block = BasicBlock::Create(this->llvmContext, "entry");

        pushBlock(block);
        Value *retValue = root.codeGen(*this);
        ReturnInst::Create(llvmContext, block);
        popBlock();

        cout << "Code generate success" << endl;

        PassManager passManager;
        passManager.add(createPrintModulePass(outs()));
        passManager.run(*module);
    }

    /* Returns an LLVM type based on the identifier */
    Type *typeOf(const NIdentifier &type)
    {
        if (type.name.compare("int") == 0) {
            return Type::getInt64Ty(llvmContext);
        } else if (type.name.compare("double") == 0) {
            return Type::getDoubleTy(llvmContext);
        }
        return Type::getVoidTy(llvmContext);
    }

    GenericValue runCode()
    {
        vector<GenericValue> noargs;
        ExecutionEngine *ee =
            EngineBuilder(unique_ptr<Module>(module)).create();

        cout << "Running code..." << endl;

        ee->finalizeObject();
        GenericValue v = ee->runFunction(mainFunction, noargs);

        std::cout << "Code was run." << endl;
        return v;
    }
};

Value *NInteger::codeGen(CodeGenContext &context)
{
    std::cout << "Creating integer: " << value << endl;
    return ConstantInt::get(Type::getInt64Ty(context.llvmContext), value, true);
}

Value *NDouble::codeGen(CodeGenContext &context)
{
    std::cout << "Creating double: " << value << endl;
    return ConstantFP::get(Type::getDoubleTy(context.llvmContext), value);
}

Value *NIdentifier::codeGen(CodeGenContext &context)
{
    std::cout << "Creating identifier reference: " << name << endl;
    if (context.locals().find(name) == context.locals().end()) {
        std::cerr << "undeclared variable " << name << endl;
        return NULL;
    }
    return new LoadInst(context.locals()[name], "", false,
                        context.currentBlock());
}

Value *NMethodCall::codeGen(CodeGenContext &context)
{
    Function *function = context.module->getFunction(id->name.c_str());
    if (function == NULL)
        std::cerr << "no such function " << id->name << endl;

    std::vector<Value *> args;
    ExpressionList::const_iterator it;

    for (it = arguments->begin(); it != arguments->end(); it++) {
        args.push_back((*it)->codeGen(context));
    }

    CallInst *call = CallInst::Create(function, makeArrayRef(args), "",
                                      context.currentBlock());
    std::cout << "Creating method call: " << id->name << endl;
    return call;
}

Value *NBinaryOperator::codeGen(CodeGenContext &context)
{
    Instruction::BinaryOps instr;

    cout << "Creating binary operation " << op << endl;
    switch (op) {
    case T_PLUS:
        instr = Instruction::Add;
        goto math;
    case T_MINUS:
        instr = Instruction::Sub;
        goto math;
    }

    return NULL;
math:
    return BinaryOperator::Create(instr, lhs->codeGen(context),
                                  rhs->codeGen(context), "",
                                  context.currentBlock());
}

Value *NAssignment::codeGen(CodeGenContext &context)
{
    cout << "Creating assignment for " << lhs->name << endl;

    if (context.locals().find(lhs->name) == context.locals().end()) {
        cerr << "undeclared variable " << lhs.name << endl;
        return NULL;
    }

    return new StoreInst(rhs->codeGen(context), context.locals()[lhs->name],
                         false, context.currentBlock());
}

Value *NBlock::codeGen(CodeGenContext &context)
{
    StatementList::const_iterator it;
    Value *last = NULL;

    for (it = statements->begin(); it != statements->end(); it++) {
        cout << "Generating code for " << typeid(*it)->name() << endl;
        last = (*it)->codeGen(context);
    }

    cout << "Creating block" << endl;
    return last;
}

Value *NExpressionStatement::codeGen(CodeGenContext &context)
{
    cout << "Generating code for " << typeid(*expression).name() << endl;
    return expression.codeGen(context);
}

Value *NReturnStatement::codeGen(CodeGenContext &context)
{
    cout << "Generating return code for " << typeid(*expression).name() << endl;
    Value *returnValue = expression->codeGen(context);
    context.setCurrentReturnValue(returnValue);
    return returnValue;
}

Value *NVariableDeclaration::codeGen(CodeGenContext &context)
{
    cout << "Creating variable declaration " << type.name << " " << id->name
         << endl;
    AllocaInst *alloc = new AllocaInst(context.typeOf(type), id->name.c_str(),
                                       context.currentBlock());
    context.locals()[id->name] = alloc;

    if (assignmentExpr != NULL) {
        NAssignment assn(*id, *assignmentExpr);
        assn.codeGen(context);
    }

    return alloc;
}

Value *NExternDeclaration::codeGen(CodeGenContext &context)
{
    vector<Type *> argTypes;
    VariableList::const_iterator it;

    for (it = arguments->begin(); it != arguments->end(); it++)
        argTypes.push_back(context.typeOf((*it)->type));

    FunctionType *ftype =
        FunctionType::get(context.typeOf(type), makeArrayRef(argTypes), false);
    Function *function = Function::Create(ftype, GlobalValue::ExternalLinkage,
                                          id->name.c_str(), context.module);
    return function;
}

Value *NFunctionDeclaration::codeGen(CodeGenContext &context)
{
    vector<Type *> argTypes;
    VariableList::const_iterator it;

    for (it = arguments->begin(); it != arguments->end(); it++)
        argTypes.push_back(context.typeOf((*it)->type));

    FunctionType *ftype =
        FunctionType::get(context.typeOf(type), makeArrayRef(argTypes), false);
    Function *function = Function::Create(ftype, GlobalValue::InternalLinkage,
                                          id->name.c_str(), context.module);
    BasicBlock *bblock = BasicBlock::Create(llvmContext, "entry", function, 0);

    context.pushBlock(bblock);

    Function::arg_iterator argsValues = function->arg_begin();
    Value *argumentValue;

    for (it = arguments->begin(); it != arguments->end(); it++) {
        (*it)->codeGen(context);

        argumentValue = &*argsValues++;
        argumentValue->setName((*it)->id->name.c_str());
        StoreInst *inst = new StoreInst(
            argumentValue, context.locals()[(*it)->id->name], false, bblock);
    }

    block->codeGen(context);
    ReturnInst::Create(llvmContext, context.getCurrentReturnValue(), block);

    context.popBlock();
    cout << "Creating function: " << id->name << endl;
    return function;
}

#endif /* __CODEGEN_H__ */
