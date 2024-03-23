#include "backend.h"
#include <locale.h>

int main(int argc, char* argv[])
{    
    ProgramNameTables nametables = {};
    
    Tree tree = {};
    TreeCtor(&tree, SYNTreeElemPrinter);

    const char* ast_file_name = (argc > 2)? argv[1] : "../../frontend/src/database.txt"; 

    ReadTreeAndNameTables(&nametables, &tree, ast_file_name);

    TreeGraphPrint(&tree, "expr_tree");

    const char* assemblered_file_name = "assemblered.txt";
    FILE* assemblered_file            = fopen(assemblered_file_name, "w");

    RetranslateTree(&tree, &nametables, assemblered_file);

    ProgramNameTablesDtor(&nametables);
    TreeDtor(&tree);

    return 0;
}    