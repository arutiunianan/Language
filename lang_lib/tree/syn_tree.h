#ifndef SYN_TREE_H_
#define SYN_TREE_H_

#include "../../frontend/lex_anal.h"

char* SYNTreeElemPrinter(const TreeNode_t* elem_to_print);
void SYNTreeElemCtor(TreeNode_t* new_elem, TreeNode_t* new_data);
void SYNTreeElemDtor(TreeNode_t* elem_to_delete);

void ReadSYNTree(Tree* tree, ProgramNameTables* nametables, FILE* syn_file, unsigned* errors);
void WriteSYNTree(Tree* tree, FILE* logger);

#endif // #define SYN_TREE_H_