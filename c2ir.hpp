#ifndef __C2IR_H__
#define __C2IR_H__

#include <iostream>
#include <llvm/Value.h>
#include <vector>

class CodeGenContext;
class Statement;
class Expression;
class VariableDeclaration;

typedef std::vector<Statement *> StatementList;
typedef std::vector<Expression *> ExpressionList;
typedef std::vector<VariableDeclaration *> VariableList;

class Node {
public:
    virtual ~Node()
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext context)
    {
        return NULL;
    }
}

class Expression : public Node {
};

class Statement : public Node {
};

class Integer : public Expression {
public:
    long long value;
    Integer(long long value)
        : value(value)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class Double : public Expression {
public:
    double value;
    Double(double value)
        : value(value)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class Identifier : public Expression {
public:
    std::string name;
    Identifier(const std::string &name)
        : name(name)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class MethodCall : public Expression {
public:
    const Identifier &id;
    ExpressionList arguments;
    MethodCall(const Identifier &id, ExpressionList &arguments)
        : id(id)
        , arguments(arguments)
    {
    }
    MethodCall(const Identifier &id)
        : id(id)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class BinaryOperator : public Expression {
public:
    int op;
    Expression &lhs;
    Expression &rhs;
    BinaryOperator(Expression &lhs, int op, Expression &rhs)
        : lhs(lhs)
        , rhs(rhs)
        , op(op)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class Assignment : public Expression {
public:
    Identifier &lhs;
    Expression &rhs;
    Assignment(Identifier &lhs, Expression &rhs)
        : lhs(lhs)
        , rhs(rhs)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class Block : public Expression {
public:
    StatementList statements;
    Block()
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class ExpressioStatement : public Statement {
public:
    Expression &expression;
    ExpressioStatement(Expression &expression)
        : expression(expression)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class ReturStatement : public Statement {
public:
    Expression &expression;
    ReturStatement(Expression &expression)
        : expression(expression)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class VariableDeclaration : public Statement {
public:
    const Identifier &type;
    Identifier &id;
    Expression *assignmentExpr;
    VariableDeclaration(const Identifier &type, Identifier &id)
        : type(type)
        , id(id)
    {
        assignmentExpr = NULL;
    }
    VariableDeclaration(const Identifier &type, Identifier &id,
                        Expression *assignmentExpr)
        : type(type)
        , id(id)
        , assignmentExpr(assignmentExpr)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class ExternDeclaration : public Statement {
public:
    const Identifier &type;
    const Identifier &id;
    VariableList arguments;
    ExternDeclaration(const Identifier &type, const Identifier &id,
                      const VariableList &arguments)
        : type(type)
        , id(id)
        , arguments(arguments)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class FunctionDeclaration : public Statement {
public:
    const Identifier &type;
    const Identifier &id;
    VariableList arguments;
    Block &block;
    FunctionDeclaration(const Identifier &type, const Identifier &id,
                        const VariableList &arguments, Block &block)
        : type(type)
        , id(id)
        , arguments(arguments)
        , block(block)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

#endif /* __C2IR_H__ */
