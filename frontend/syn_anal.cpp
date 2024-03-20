#include "syn_anal.h"

Tree GetCodeTree(const char* file_name, ProgramNameTables* nametables)
{ 
	assert(file_name  != nullptr);
	assert(nametables != nullptr);

	TreeNode** tokens    = nullptr;
	Tree expr_tree = {};
	TreeCtor(&expr_tree, ASTTreeElemCtor, ASTTreeElemDtor, ASTTreeElemPrinter);

	size_t scopes_counter = 0;
	tokens = DoLexicalAnalisys(&expr_tree, file_name, &nametables->common_nametable, &scopes_counter);

	ProgramNameTablesCtor(nametables, scopes_counter);

	size_t current_token_num = 0;
	expr_tree.root = GetProgram(tokens, &current_token_num, nametables);

	