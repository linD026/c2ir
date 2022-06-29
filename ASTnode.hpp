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

/* ------------------------- Expression ------------------------- */

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

class NLiteral : public NExpression {
public:
    string value;

    void print()
    {
        cout << "NLiteral: " << value << endl;
    }

    NLiteral(const string &str)
    {
        value = str.substr(1, str.length()-2);
        print();
    }

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NIdentifier : public NExpression {
public:
    string name;
    bool isType = false;
    bool isPtr = false;

    void print()
    {
        cout << "NIdentifier: " << name << endl;
    }

    NIdentifier(const string &name)
        : name(name)
    {
        print();
    }

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NMethodCall : public NExpression {
public:
    const NIdentifier *id;
    ExpressionList *arguments;

    void print()
    {
        cout << "NMethdCall" << endl;
    }

    NMethodCall(const NIdentifier *id, ExpressionList *arguments)
        : id(id)
        , arguments(arguments)
    {
        print();
    }

    NMethodCall(const NIdentifier *id)
        : id(id)
    {
        print();
    }

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NBinaryOperator : public NExpression {
public:
    int op;
    NIdentifier *lhs;
    NExpression *rhs;

    void print()
    {
        cout << "NBinaryOperator" << endl;
    }

    NBinaryOperator(NIdentifier *lhs, int op, NExpression *rhs)
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
    NIdentifier *lhs;
    NExpression *rhs;

    void print()
    {
        cout << "NAssignment" << endl;
    }

    NAssignment(NIdentifier *lhs, NExpression *rhs)
        : lhs(lhs)
        , rhs(rhs)
    {
        print();
    }
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NBlock : public NExpression {
public:
    StatementList *statements = new StatementList();

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

/* ------------------------- Statement ------------------------- */

class NExpressionStatement : public NStatement {
public:
    NExpression *expression;

    void print()
    {
        cout << "NExpressionStatment" << endl;
    }

    NExpressionStatement(NExpression *expression)
        : expression(expression)
    {
        print();
    }

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NReturnStatement : public NStatement {
public:
    NExpression *expression;
    void print()
    {
        cout << "NReturnStatement" << endl;
    }

    NReturnStatement(NExpression *expression)
        : expression(expression)
    {
        print();
    }
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NVariableDeclaration : public NStatement {
public:
    const NIdentifier *type;
    NIdentifier *id;
    NExpression *assignmentExpr = nullptr;

    void print()
    {
        cout << "NVariableDeclaration" << endl;
    }

    NVariableDeclaration(const NIdentifier *type, NIdentifier *id,
                         NExpression *assignmentExpr = NULL)
        : type(type)
        , id(id)
        , assignmentExpr(assignmentExpr)
    {
        print();
    }
    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NFunctionDeclaration : public NStatement {
public:
    const NIdentifier *type = NULL;
    const NIdentifier *id = NULL;
    VariableList *arguments;
    NBlock *block = NULL;
    bool isExtern = false;

    void print()
    {
        if (isExtern)
            cout << "Extern NFunctionDeclaration" << endl;
        else
            cout << "NFunctionDeclaration" << endl;
    }

    NFunctionDeclaration(const NIdentifier *type, const NIdentifier *id,
                         VariableList *arguments, NBlock *block)
        : type(type)
        , id(id)
        , arguments(arguments)
        , block(block)
    {
        if (block == nullptr)
            isExtern = true;
        print();
    }

    virtual llvm::Value *codeGen(CodeGenContext &context);
};

#endif /* __ASTNODE_H__ */
