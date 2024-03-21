#include "syn_anal.h"

#include <locale.h>

int main(int argc, char* argv[])
{    
    ProgramNameTables nametables = {};

    Tree tree = GetCodeTree((argc > 1)? argv[1] : "exm.txt", &nametables);

    TreeGraphPrint(&tree, "expr_tree");
    
    FILE* ast_file = fopen((argc > 2)? argv[2] : "database.txt", "w");

    WriteProgramNameTables(&nametables, ast_file);
    WriteSYNTree(&tree, ast_file);

    ProgramNameTablesDtor(&nametables);
    TreeDtor(&tree);

    return 0;
}    