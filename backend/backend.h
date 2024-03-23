#ifndef BACKEND_H
#define BACKEND_H

#include "../lang_lib/tree/syn_tree.h"

const char const MAIN_FUNC_NAME[] = "пупупу";

void RetranslateTree(Tree* tree, ProgramNameTables* nametables, FILE* asm_code_file);

void ReadTreeAndNameTables(ProgramNameTables* nametables, Tree* tree, const char* ast_file_name);

#endif // BACKEND_H