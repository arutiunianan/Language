#include "syn_anal.h"

Tree GetCodeTree(const char* file_name, ProgramNameTables* nametables)
{ 
	assert(file_name  != NULL);
	assert(nametables != NULL);

	TreeNode** tokens = NULL;
	Tree expr_tree    = {};
	TreeCtor(&expr_tree, SYNTreeElemPrinter);

	size_t locals_counter = 0;
	tokens = DoLexicalAnalisys(&expr_tree, file_name, &nametables->common_nametable, &locals_counter);

	ProgramNameTablesCtor(nametables, locals_counter);

	size_t current_token_num = 0;
	//expr_tree.root = GetProgram(tokens, &current_token_num, nametables);

	return expr_tree;
}