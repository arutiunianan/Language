#include "syn_anal.h"

/*
int SkipTabs(char* code, int if_number)
{
    assert(code != NULL);
    assert(if_number >= 0);
    for(int i = 0; i < if_number * 4; i++)
    {
        if(code[i] != ' ')
        {
            return 1;
        }
    }
    return 0;
}
*/

#include <locale.h>

int main(int argc, char* argv[])
{	
	ProgramNameTables nametables = {};
printf("дошел");
	Tree tree = GetCodeTree((argc > 1)? argv[1] : "exm.txt", &nametables);

	TreeGraphPrint(&tree, "expr_tree");
    
	FILE* ast_file = fopen((argc > 2)? argv[2] : "database.txt", "w");

	WriteProgramNameTables(&nametables, ast_file);
	WriteSYNTree(&tree, ast_file);

	ProgramNameTablesDtor(&nametables);
	TreeDtor(&tree);

	return 0;
}	