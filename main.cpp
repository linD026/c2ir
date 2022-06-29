#include <iostream>

#include "codegen.hpp"
#include "ASTnode.hpp"

#include "objgen.hpp"
#include "corefn.hpp"

using namespace std;

extern int yyparse();
extern NBlock *programBlock;

int main(int argc, char **argv)
{
    yyparse();
    cout << programBlock << endl;

    cout << "---------------------" << endl;

    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

    CodeGenContext context;
    createCoreFunctions(context);
    context.generateCode(*programBlock);

    cout << "---------------------" << endl;

    ObjGen(context, "text.o");

    return 0;
}
