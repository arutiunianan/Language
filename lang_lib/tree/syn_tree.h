#ifndef SYN_TREE_H_
#define SYN_TREE_H_

#include "../str_lib/io.h"
#include "tree.h"

char* SYNTreeElemPrinter(const TreeNode_t* elem_to_print);
void SYNTreeElemCtor(TreeNode_t* new_elem, TreeNode_t* new_data);
void SYNTreeElemDtor(TreeNode_t* elem_to_delete);

void ReadSYNTree(Tree* tree, ProgramNameTables* nametables, FILE* syn_file, int* errors);
void WriteSYNTree(Tree* tree, FILE* logger);

int IsOper(const char* design);
Operation GetOperValue(const char* design);
char* GetOperDesignation(Operation oper);
TreeNode* CreateNumNode(Tree* expr_tree, double num);
TreeNode* CreateOperNode(Tree* expr_tree, Operation oper, TreeNode* left_node, TreeNode* right_node);
TreeNode* CreateIdNode(Tree* expr_tree, NameTableElem* nametable_elem_ptr);

#endif // #define SYN_TREE_H_