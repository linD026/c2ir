#ifndef __COREFN_H__
#define __COREFN_H__

#include <iostream>
#include "codegen.hpp"
#include "ASTnode.hpp"

using namespace std;

extern int yyparse();
extern NBlock *programBlock;

llvm::Function *createPrintfFunction(CodeGenContext &context)
{
    std::vector<llvm::Type *> printf_arg_types;
    printf_arg_types.push_back(
        llvm::Type::getInt8PtrTy(context.llvmContext)); //char*

    llvm::FunctionType *printf_type = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context.llvmContext), printf_arg_types, true);

    llvm::Function *func =
        llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage,
                               llvm::Twine("printf"), context.module);
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

void createEchoFunction(CodeGenContext &context, llvm::Function *printfFn)
{
    std::vector<llvm::Type *> echo_arg_types;
    echo_arg_types.push_back(llvm::Type::getInt64Ty(context.llvmContext));

    llvm::FunctionType *echo_type = llvm::FunctionType::get(
        llvm::Type::getVoidTy(context.llvmContext), echo_arg_types, false);

    llvm::Function *func =
        llvm::Function::Create(echo_type, llvm::Function::InternalLinkage,
                               llvm::Twine("echo"), context.module);
    llvm::BasicBlock *bblock =
        llvm::BasicBlock::Create(context.llvmContext, "entry", func, 0);
    context.pushBlock(bblock);

    const char *constValue = "%d\n";
    llvm::Constant *format_const =
        llvm::ConstantDataArray::getString(context.llvmContext, constValue);
    llvm::GlobalVariable *var = new llvm::GlobalVariable(
        *context.module,
        llvm::ArrayType::get(llvm::IntegerType::get(context.llvmContext, 8),
                             strlen(constValue) + 1),
        true, llvm::GlobalValue::PrivateLinkage, format_const, ".str");
    llvm::Constant *zero = llvm::Constant::getNullValue(
        llvm::IntegerType::getInt32Ty(context.llvmContext));

    std::vector<llvm::Constant *> indices;
    indices.push_back(zero);
    indices.push_back(zero);
    llvm::Constant *var_ref = llvm::ConstantExpr::getGetElementPtr(
        llvm::ArrayType::get(llvm::IntegerType::get(context.llvmContext, 8),
                             strlen(constValue) + 1),
        var, indices);

    std::vector<Value *> args;
    args.push_back(var_ref);

    Function::arg_iterator argsValues = func->arg_begin();
    Value *toPrint = &*argsValues++;
    toPrint->setName("toPrint");
    args.push_back(toPrint);

    CallInst *call = CallInst::Create(printfFn, makeArrayRef(args), "", bblock);
    ReturnInst::Create(context.llvmContext, bblock);
    context.popBlock();
}

void createCoreFunctions(CodeGenContext &context)
{
    llvm::Function *printfFn = createPrintfFunction(context);
    createEchoFunction(context, printfFn);
}

#endif
