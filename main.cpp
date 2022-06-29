#include <iostream>

#include "codegen.hpp"
#include "ASTnode.hpp"

#include "ObjGen.hpp"
#include "corefn.hpp"

using namespace std;

extern int yyparse();
extern NBlock *programBlock;

int main(int argc, char **argv)
{
    yyparse();
    std::cout << programBlock << std::endl;

    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

    CodeGenContext context;
    createCoreFunctions(context);
    context.generateCode(*programBlock);
    ObjGen(context, "text.o");
    context.runCode();

    return 0;
}
