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
};

class CodeGenContext {

};

Value *NInteger::codeGen(CodeGenContext &context)
{
    return NULL;
}

Value *NLiteral::codeGen(CodeGenContext &context)
{
    return NULL;
}

Value *NIdentifier::codeGen(CodeGenContext &context)
{
    return NULL;
}

Value *NMethodCall::codeGen(CodeGenContext &context)
{
    return NULL;
}

Value *NBinaryOperator::codeGen(CodeGenContext &context)
{
    return NULL;
}

Value *NAssignment::codeGen(CodeGenContext &context)
{
    return NULL;
}

Value *NBlock::codeGen(CodeGenContext &context)
{
    return NULL;
}

Value *NExpressionStatement::codeGen(CodeGenContext &context)
{
    return NULL;
}

Value *NReturnStatement::codeGen(CodeGenContext &context)
{
    return NULL;
}

Value *NVariableDeclaration::codeGen(CodeGenContext &context)
{
    return NULL;
}

Value *NFunctionDeclaration::codeGen(CodeGenContext &context)
{
    return NULL;
}

#endif /* __CODEGEN_H__ */
