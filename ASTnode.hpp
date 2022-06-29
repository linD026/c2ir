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

typedef vector<NStatement *> StatementList;
typedef vector<NExpression *> ExpressionList;
typedef vector<NVariableDeclaration *> VariableList;

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

    void print()
    {
        cout << "NInteger: " << value << endl;
    }

    NInteger(long long value)
        : value(value)
    {
        print();
    }

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NIdentifier : public NExpression {
public:
    std::string name;
    bool isType = false;

    void print()
    {
        cout << "NIdentifier: " << name << endl;
    }

    NIdentifier(const std::string &name)
        : name(name)
    {
        print();
    }

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NMethodCall : public NExpression {
public:
    const NIdentifier &id;
    ExpressionList arguments;

    void print()
    {
        cout << "NMethdCall" << endl;
    }

    NMethodCall(const NIdentifier &id, ExpressionList &arguments)
        : id(id)
        , arguments(arguments)
    {
        print();
    }

    NMethodCall(const NIdentifier &id)
        : id(id)
    {
        print();
    }

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NBinaryOperator : public NExpression {
public:
    int op;
    NExpression &lhs;
    NExpression &rhs;

    void print()
    {
        cout << "NBinaryOperator" << endl;
    }

    NBinaryOperator(NExpression &lhs, int op, NExpression &rhs)
        : lhs(lhs)
        , rhs(rhs)
        , op(op)
    {
        print();
    }

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NAssignment : public NExpression {
public:
    NExpression &lhs;
    NExpression &rhs;

    void print()
    {
        cout << "NAssignment" << endl;
    }

    NAssignment(NIdentifier &lhs, NExpression &rhs)
        : lhs(lhs)
        , rhs(rhs)
    {
        print();
    }
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NBlock : public NExpression {
public:
    StatementList statements;

    void print()
    {
        cout << "NBlock" << endl;
    }

    NBlock()
    {
        print();
    }
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NExpressionStatement : public NStatement {
public:
    NExpression &expression;

    void print()
    {
        cout << "NExpressionStatment" << endl;
    }

    NExpressionStatement(NExpression &expression)
        : expression(expression)
    {
        print();
    }

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NReturnStatement : public NStatement {
public:
    NExpression &expression;
    void print()
    {
        cout << "NReturnStatement" << endl;
    }

    NReturnStatement(NExpression &expression)
        : expression(expression)
    {
        print();
    }
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NVariableDeclaration : public NStatement {
public:
    const NIdentifier &type;
    NIdentifier &id;
    NExpression *assignmentExpr = nullptr;

    void print()
    {
        cout << "NVariableDeclaration" << endl;
    }

    NVariableDeclaration(const NIdentifier &type, NIdentifier &id,
                         NExpression *assignmentExpr)
        : type(type)
        , id(id)
        , assignmentExpr(assignmentExpr)
    {
        print();
    }
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NExternDeclaration : public NStatement {
public:
    const NIdentifier &type;
    const NIdentifier &id;
    VariableList arguments;
    void print()
    {
        cout << "NExternDeclaration" << endl;
    }

    NExternDeclaration(const NIdentifier &type, const NIdentifier &id,
                       const VariableList &arguments)
        : type(type)
        , id(id)
        , arguments(arguments)
    {
        print();
    }
    virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NFunctionDeclaration : public NStatement {
public:
    const NIdentifier &type;
    const NIdentifier &id;
    VariableList arguments;
    NBlock &block;
    bool isExtern = false;

    void print()
    {
        cout << "NFunctionDeclaration" << endl;
    }

    NFunctionDeclaration(const NIdentifier &type, const NIdentifier &id,
                         const VariableList &arguments)
        : type(type)
        , id(id)
        , arguments(arguments)
        , block(*(new NBlock()))
    {
        isExtern = true;
        print();
    }

    NFunctionDeclaration(const NIdentifier &type, const NIdentifier &id,
                         const VariableList &arguments, NBlock &block)
        : type(type)
        , id(id)
        , arguments(arguments)
        , block(block)
    {
        print();
    }

    virtual llvm::Value *codeGen(CodeGenContext &context);
};

#endif /* __ASTNODE_H__ */
