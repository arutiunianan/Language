#ifndef SYN_ANAL_H
#define SYN_ANAL_H

#include "lex_anal.h"

Tree GetCodeTree(const char* file_name, ProgramNameTables* nametables);
void PrintCurrentToken(TreeNode** tokens, int* current_token_num);

#endif // SYN_ANAL_H