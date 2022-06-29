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

class CodeGenBlock {
public:
    BasicBlock *block;
    Value *returnValue;
    std::map<string, NIdentifier *> types;
    std::map<string, bool> isFuncArg;
    std::map<string, Value *> locals;
};

class CodeGenContext {
    std::stack<CodeGenBlock *> blocks;
    Function *mainFunction;

public:
    LLVMContext llvmContext;
    IRBuilder<> builder;
    Module *module;

    CodeGenContext()
        : builder(llvmContext)
    {
        module = new Module("main", llvmContext);
    }

    BasicBlock *currentBlock()
    {
        return blocks.top()->block;
    }

    std::map<std::string, Value *> &locals()
    {
        return blocks.top()->locals;
    }

    void pushBlock(BasicBlock *block)
    {
        CodeGenBlock *codeGenBlock = new CodeGenBlock();
        blocks.push(codeGenBlock);
        blocks.top()->returnValue = nullptr;
        blocks.top()->block = block;
    }

    void popBlock()
    {
        CodeGenBlock *top = blocks.top();
        blocks.pop();
        delete top;
    }

    void setCurrentReturnValue(Value *value)
    {
        blocks.top()->returnValue = value;
    }

    Value *getCurrentReturnValue()
    {
        return blocks.top()->returnValue;
    }

    /* Compile the AST into a module */

    void generateCode(NBlock &root)
    {
        cout << "Generating code..." << endl;

        /* Create the top level interpreter function to call as entry */
        vector<Type *> argTypes;
        FunctionType *ftype = FunctionType::get(Type::getVoidTy(llvmContext),
                                                makeArrayRef(argTypes), false);
        mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage,
                                        "main", module);
        BasicBlock *bblock =
            BasicBlock::Create(llvmContext, "entry", mainFunction, 0);

        cout << "Start generate code" << endl;

        /* Push a new variable/block context */
        pushBlock(bblock);

        cout << "After push Block" << endl;

        root.codeGen(*this); /* emit bytecode for the toplevel block */

        cout << "After code gen" << endl;

        ReturnInst::Create(llvmContext, bblock);
        popBlock();

        /*
         * Print the bytecode in a human-readable format 
	     * to see if our program compiled properly
	     */
        cout << "Code is generated." << endl;
        // module->dump();

        legacy::PassManager pm;
        pm.add(createPrintModulePass(outs()));
        pm.run(*module);
    }

    /* Executes the AST by running the main function */
    GenericValue runCode()
    {
        cout << "Running code..." << endl;
        ExecutionEngine *ee =
            EngineBuilder(unique_ptr<Module>(module)).create();
        ee->finalizeObject();
        vector<GenericValue> noargs;
        GenericValue v = ee->runFunction(mainFunction, noargs);
        cout << "Code was run." << endl;
        return v;
    }

    /* Returns an LLVM type based on the identifier */
    Type *TypeOf(const NIdentifier &type)
    {
        cout << "     identifier type: " + type.name << endl;
        if (type.name.compare("int") == 0) {
            return Type::getInt32Ty(llvmContext);
        } else if (type.name.compare("char") == 0) {
            if (type.isPtr)
                return Type::getInt8PtrTy(llvmContext);
            return Type::getInt8Ty(llvmContext);
        }
        cout << "should not be here, since text.c won't have void type variable" << endl;
        return Type::getVoidTy(llvmContext);
    }

    Value *getSymbolValue(string name) const
    {
        if (blocks.top()->locals.find(name) != blocks.top()->locals.end())
            return blocks.top()->locals[name];
        return nullptr;
    }

    void setSymbolValue(string name, Value *value)
    {
        blocks.top()->locals[name] = value;
    }

    void setSymbolType(string name, NIdentifier *value)
    {
        blocks.top()->types[name] = value;
    }

    void setFuncArg(string name, bool value)
    {
        cout << "Set " << name << " as func arg" << endl;
        blocks.top()->isFuncArg[name] = value;
    }
};

static inline string llvmTypeToStr(Value *value)
{
    Type::TypeID typeID;
    Type *type = value->getType();
    if (type)
        typeID = type->getTypeID();
    else
        return "type is nullptr";

    switch (typeID) {
    case Type::VoidTyID:
        return "VoidTyID";
    case Type::HalfTyID:
        return "HalfTyID";
    case Type::FloatTyID:
        return "FloatTyID";
    case Type::DoubleTyID:
        return "DoubleTyID";
    case Type::IntegerTyID:
        return "IntegerTyID";
    case Type::FunctionTyID:
        return "FunctionTyID";
    case Type::StructTyID:
        return "StructTyID";
    case Type::ArrayTyID:
        return "ArrayTyID";
    case Type::PointerTyID:
        return "PointerTyID";
    case Type::VectorTyID:
        return "VectorTyID";
    default:
        return "Unknown";
    }
}

Value *NInteger::codeGen(CodeGenContext &context)
{
    cout << "Generating Integer: " << this->value << endl;

    return ConstantInt::get(Type::getInt32Ty(context.llvmContext), this->value,
                            true);
}

Value *NLiteral::codeGen(CodeGenContext &context)
{
    cout << "Generating Literal: " << this->value << endl;
    return context.builder.CreateGlobalString(this->value, "string");
}

Value *NIdentifier::codeGen(CodeGenContext &context)
{
    cout << "Generating identifier " << this->name << endl;

    //Value *value =
    //    new LoadInst(context.locals()[name], "", false, context.currentBlock());

    Value *value = context.getSymbolValue(this->name);
    if (!value)
        cout << "Unknown variable name " + this->name << endl;

    if (value->getType()->isPointerTy()) {
        auto arrayPtr = context.builder.CreateLoad(value, "arrayPtr");
        if (arrayPtr->getType()->isArrayTy()) {
            cout << "(Array Type)" << endl;
            //            arrayPtr->setAlignment(16);
            std::vector<Value *> indices;
            indices.push_back(ConstantInt::get(
                Type::getInt32Ty(context.llvmContext), 0, false));
            auto ptr =
                context.builder.CreateInBoundsGEP(value, indices, "arrayPtr");
            return ptr;
        }
    }

    return context.builder.CreateLoad(value, false, "");
}

Value *NMethodCall::codeGen(CodeGenContext &context)
{
    cout << "Generating method call of " << this->id->name << endl;

    Function *calleeF = context.module->getFunction(this->id->name);
    std::vector<Value *> argsv;

    if (!calleeF)
        cout << "calleef NULL" << endl;
    if (calleeF->arg_size() != arguments->size())
        cout << "Function arguments size not match, calleeF=" +
                    std::to_string(calleeF->size()) + ", this->arguments=" +
                    std::to_string(this->arguments->size())
             << endl;

    cout << "    Start of callee arg" << endl;

    for (auto it = arguments->begin(); it != arguments->end(); it++) {
        argsv.push_back((*it)->codeGen(context));
        if (!argsv.back()) { // if any argument codegen fail
            cout << "    arg codegen fail" << endl;
            return nullptr;
        }
    }

    cout << "    End of callee arg" << endl;

    return context.builder.CreateCall(calleeF, argsv, "calltmp");
}

Value *NBinaryOperator::codeGen(CodeGenContext &context)
{
    cout << "Generating binary operator" << endl;

    Value *L = this->lhs->codeGen(context);
    Value *R = this->rhs->codeGen(context);

    if (!L || !R)
        return nullptr;

    cout << "L is " << llvmTypeToStr(L) << endl;
    cout << "R is " << llvmTypeToStr(R) << endl;

    switch (this->op) {
    case T_ADD:
        return context.builder.CreateAdd(L, R, "addtmp");
    case T_MINUS:
        return context.builder.CreateSub(L, R, "subtmp");
    default:
        return nullptr;
    }
}

Value *NAssignment::codeGen(CodeGenContext &context)
{
    cout << "Generating assignment of " << this->lhs->name << " = " << endl;

    if (context.locals().find(lhs->name) == context.locals().end()) {
        cerr << "undeclared variable " << lhs->name << endl;
        return NULL;
    }

    Value *exp = this->rhs->codeGen(context);
    Value *dst = context.getSymbolValue(this->lhs->name);
    //Value *dst = context.locals()[lhs->name];

    return context.builder.CreateStore(exp, dst);
    // new StoreInst(rhs.codeGen(context), context.locals()[lhs.name],
    //                  false, context.currentBlock());
}

Value *NBlock::codeGen(CodeGenContext &context)
{
    cout << "Generating block" << endl;
    Value *last = nullptr;
    StatementList::const_iterator it;

    cout << "    Start of block" << endl;
    for (it = statements->begin(); it != statements->end(); it++) {
        std::cout << "      Generating code for " << typeid(**it).name()
                  << endl;
        last = (*it)->codeGen(context);
    }
    cout << "    End of block" << endl;
    return last;
}

Value *NExpressionStatement::codeGen(CodeGenContext &context)
{
    return this->expression->codeGen(context);
}

Value *NReturnStatement::codeGen(CodeGenContext &context)
{
    cout << "Generating return statement" << endl;
    Value *returnValue = this->expression->codeGen(context);
    context.setCurrentReturnValue(returnValue);
    return returnValue;
}

Value *NVariableDeclaration::codeGen(CodeGenContext &context)
{
    cout << "Generating variable declaration of " << this->type->name
         << ((this->type->isPtr) ? " *" : " ") << " " << this->id->name << endl;
    Type *type = context.TypeOf(*this->type);
    Value *initial = nullptr;

    Value *inst = nullptr;

    inst = context.builder.CreateAlloca(type);

    context.setSymbolType(this->id->name, (NIdentifier *)this->type);
    context.setSymbolValue(this->id->name, inst);

    if (this->assignmentExpr != nullptr) {
        NAssignment assignment(this->id, this->assignmentExpr);
        assignment.codeGen(context);
    }
    return inst;
}

Value *NFunctionDeclaration::codeGen(CodeGenContext &context)
{
    cout << "Generating function declaration of " << this->id->name << endl;
    std::vector<Type *> argTypes;

    for (auto &arg : *this->arguments)
        argTypes.push_back(context.TypeOf(*arg->type));

    Type *retType = nullptr;
    retType = context.TypeOf(*this->type);

    FunctionType *functionType = FunctionType::get(retType, argTypes, false);
    Function *function =
        Function::Create(functionType, GlobalValue::ExternalLinkage,
                         this->id->name.c_str(), context.module);

    if (!this->isExtern) {
        BasicBlock *basicBlock =
            BasicBlock::Create(context.llvmContext, "entry", function, nullptr);

        context.builder.SetInsertPoint(basicBlock);
        context.pushBlock(basicBlock);

        cout << "  start of arguments" << endl;

        Function::arg_iterator argsValues = function->arg_begin();
        Value *argumentValue;
        for (VariableList::const_iterator it = arguments->begin();
             it != arguments->end(); it++) {
            (**it).codeGen(context);

            argumentValue = &*argsValues++;
            argumentValue->setName((*it)->id->name.c_str());
            StoreInst *inst =
                new StoreInst(argumentValue, context.locals()[(*it)->id->name],
                              false, basicBlock);
        }

        cout << "  End of arguments" << endl;

        this->block->codeGen(context);

        cout << "Function block created" << endl;

        if (context.getCurrentReturnValue())
            context.builder.CreateRet(context.getCurrentReturnValue());
        context.popBlock();
    }

    return function;
}

#endif /* __CODEGEN_H__ */
