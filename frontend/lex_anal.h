#ifndef LEX_ANAL_H
#define LEX_ANAL_H

#include "../lang_lib/str_lib/io.h"
#include "../lang_lib/tree/tree.h"

TreeNode** DoLexicalAnalisys(Tree* expr_tree, const char* file_name, NameTable* common_nametable, size_t* functions_counter);

char* GetOperDesignation(Operation oper);
Operation GetOperValue(const char* design);
int IsOper(const char* design);
TreeNode* CreateNumNode (Tree* expr_tree, double num);
TreeNode* CreateOperNode(Tree* expr_tree, Operation oper, TreeNode* left_node, TreeNode* right_node);
TreeNode* CreateIdNode  (Tree* expr_tree, NameTableElem* nametable_elem_ptr);

#endif // LEX_ANAL_H