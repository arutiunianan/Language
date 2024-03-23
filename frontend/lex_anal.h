#ifndef LEX_ANAL_H
#define LEX_ANAL_H

#include "../lang_lib/tree/syn_tree.h"

TreeNode** DoLexicalAnalisys(Tree* expr_tree, const char* file_name, NameTable* common_nametable, int* functions_counter);

#endif // LEX_ANAL_H