#ifndef SYN_ANAL_H
#define SYN_ANAL_H

#include "../lang_lib/tree/syn_tree.h"

Tree GetCodeTree(const char* file_name, ProgramNameTables* nametables);
void PrintCurrentToken(TreeNode** tokens, size_t* current_token_num);

#endif // SYN_ANAL_H