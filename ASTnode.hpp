#ifndef __ASTNODE_H__
#define __ASTNODE_H__

#include <iostream>
#include <llvm/IR/Value.h>
#include <vector>

using namespace std;

class CodeGenContext;
class NBlock;
class NStatement;
class NExpression;
class NVariableDeclaration;

typedef vector<shared_ptr<NStatement> > StatementList;
typedef vector<shared_ptr<NExpression> > ExpressionList;
typedef vector<shared_ptr<NVariableDeclaration> > VariableList;

class Node {
public:
    Node()
    {
    }
    virtual ~Node()
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context)
    {
        return (llvm::Value *)0;
    }
};

class NExpression : public Node {
public:
    NExpression()
    {
    }
};

class NStatement : public Node {
public:
    NStatement()
    {
    }
};

class NInteger : public NExpression {
public:
    long long value;

    NInteger(long long value)
        : value(value)
    {
    }

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NDouble : public NExpression {
public:
    double value;

    NDouble(double value)
        : value(value)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NIdentifier : public NExpression {
public:
    std::string name;

    NIdentifier(const std::string &name)
        : name(name)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NMethodCall : public NExpression {
public:
    const shared_ptr<NIdentifier> id;
    shared_ptr<ExpressionList> arguments = make_shared<ExpressionList>();

    NMethodCall(const shared_ptr<NIdentifier> id,
                shared_ptr<ExpressionList> arguments)
        : id(id)
        , arguments(arguments)
    {
    }
    NMethodCall(const shared_ptr<NIdentifier> id)
        : id(id)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NBinaryOperator : public NExpression {
public:
    int op;
    shared_ptr<NExpression> lhs;
    shared_ptr<NExpression> rhs;

    NBinaryOperator(shared_ptr<NExpression> lhs, int op,
                    shared_ptr<NExpression> rhs)
        : lhs(lhs)
        , rhs(rhs)
        , op(op)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NAssignment : public NExpression {
public:
    shared_ptr<NIdentifier> lhs;
    shared_ptr<NExpression> rhs;

    NAssignment()
    {
    }
    NAssignment(shared_ptr<NIdentifier> lhs, shared_ptr<NExpression> rhs)
        : lhs(lhs)
        , rhs(rhs)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NBlock : public NExpression {
public:
    shared_ptr<StatementList> statements;

    NBlock()
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NExpressionStatement : public NStatement {
public:
    shared_ptr<NExpression> expression;

    NExpressionStatement(shared_ptr<NExpression> expression)
        : expression(expression)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NReturnStatement : public NStatement {
public:
    shared_ptr<NExpression> expression;

    NReturnStatement()
    {
    }

    NReturnStatement(shared_ptr<NExpression> expression)
        : expression(expression)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NVariableDeclaration : public NStatement {
public:
    const shared_ptr<NIdentifier> type;
    shared_ptr<NIdentifier> id;
    shared_ptr<NExpression> assignmentExpr = nullptr;

    NVariableDeclaration()
    {
    }
    NVariableDeclaration(const shared_ptr<NIdentifier> type,
                         shared_ptr<NIdentifier> id,
                         shared_ptr<NExpression> assignmentExpr)
        : type(type)
        , id(id)
        , assignmentExpr(assignmentExpr)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NExternDeclaration : public NStatement {
public:
    const shared_ptr<NIdentifier> type;
    const shared_ptr<NIdentifier> id;
    shared_ptr<VariableList> arguments = make_shared<VariableList>();

    NExternDeclaration()
    {
    }
    NExternDeclaration(const shared_ptr<NIdentifier> type,
                       const shared_ptr<NIdentifier> id,
                       const shared_ptr<VariableList> arguments)
        : type(type)
        , id(id)
        , arguments(arguments)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NFunctionDeclaration : public NStatement {
public:
    const shared_ptr<NIdentifier> type;
    const shared_ptr<NIdentifier> id;
    shared_ptr<VariableList> arguments = make_shared<VariableList>();
    shared_ptr<NBlock> block;

    NFunctionDeclaration()
    {
    }
    NFunctionDeclaration(const shared_ptr<NIdentifier> type,
                         const shared_ptr<NIdentifier> id,
                         const shared_ptr<VariableList> arguments,
                         shared_ptr<NBlock> block)
        : type(type)
        , id(id)
        , arguments(arguments)
        , block(block)
    {
    }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

#endif /* __ASTNODE_H__ */
