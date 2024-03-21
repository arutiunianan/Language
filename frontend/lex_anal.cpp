#include "lex_anal.h"

char* GetOperDesignation(Operation oper)
{
    switch(oper)
    {
        #define OPER_DEF(value, designation1, designation2)     \
        case value:                                              \
            return designation2;

        #include "../lang_lib/oper_defs.h"
        #undef OPER_DEF
    }
}

Operation GetOperValue(const char* design)
{
	assert(design != NULL);

	Operation oper = WRONG_OPER;

	#define OPER_DEF(value, designation1, designation2) \
	    if(strcmp(design, designation1) == 0)            \
		    return value;                                 \
        if(strcmp(design, designation2) == 0)              \
		    return value;

	#include "../lang_lib/oper_defs.h"

	#undef OPER_DEF

	return WRONG_OPER;
}

int IsOper(const char* design)
{
	assert(design != NULL);

    #define OPER_DEF(value, designation1, designation2) \
	    if(strcmp(design, designation1) == 0)            \
		    return 1;                                     \
        if(strcmp(design, designation2) == 0)              \
		    return 1;

	#include "../lang_lib/oper_defs.h"

	#undef OPER_DEF

	return 0;
}

TreeNode* CreateNumNode(Tree* expr_tree, double num)
{
	assert(expr_tree != NULL);

	TreeNode*  new_node = {};
	TreeNode_t new_data = {};

	new_data.type     = NUM;
	new_data.elem.num = num;

	return CreateNode(expr_tree, &new_data, &new_node, NULL, NULL);
}

TreeNode* CreateOperNode(Tree* expr_tree, Operation oper, TreeNode* left_node, TreeNode* right_node)
{
	assert(expr_tree != NULL);;

	TreeNode*  new_node = {};
	TreeNode_t new_data = {};

	new_data.type      = OPER;
	new_data.elem.oper = oper;

	return CreateNode(expr_tree, &new_data, &new_node, left_node, right_node);
}

TreeNode* CreateIdNode(Tree* expr_tree, NameTableElem* nametable_elem_ptr)
{
	assert(expr_tree != NULL);

	TreeNode*  new_node = {};
	TreeNode_t new_data = {};

	new_data.type    = ID;
	new_data.elem.id = nametable_elem_ptr;

	return CreateNode(expr_tree, &new_data, &new_node, NULL, NULL);
}

int isdigit(char sym)
{
    return (sym >= '0' && sym <='9' ) ? 1 : 0;
}

size_t TrySetNum(Tree* expr_tree, TreeNode** token_ptr, char* code)
{
	assert(expr_tree != NULL);
	assert(code      != NULL);

	size_t curr_ch_num                  = 0;
	double num                          = 0;
	int int_part_of_num                 = 1;
	size_t fraction_part_digits_counter = 10;

	while(isdigit(code[curr_ch_num]) || code[curr_ch_num] == '.')
	{
		if(code[curr_ch_num] == '.')
        {
			int_part_of_num = 0;
        }
		else
		{
			if(int_part_of_num)
            {
				num = num * 10 + (code[curr_ch_num] - '0');
            }
			else
			{
				num = num + (double)(code[curr_ch_num] - '0') / fraction_part_digits_counter;

				fraction_part_digits_counter *= 10;
			}
		}

		curr_ch_num++;
	}

	// Num must contain smth
	if(!curr_ch_num)
    {
		return 0;
    }

	*token_ptr = CreateNumNode(expr_tree, num);

	return curr_ch_num;
}

int IsCharLetter(char ch)
{
	return (65  <= ch && ch <= 90)  || (97   <= ch && ch <= 122) ||
		   (128 <= ch && ch <= 175) || ( 224 <= ch && ch <= 241) ||
		   ( -64 <= ch && ch <= -1);
}

// OPER SYMB ex: -, /, *, etc
size_t TrySetOperSymb(Tree* expr_tree, TreeNode** token_ptr, const char* code)
{
	assert(expr_tree != NULL);
	assert(token_ptr != NULL);
	assert(code      != NULL);

	char oper_symb[MAX_ID_SIZE] = "";
	oper_symb[0] = code[0];

	if(IsOper(oper_symb) && !IsCharLetter(oper_symb[0]))
	{
		*token_ptr = CreateOperNode(expr_tree, GetOperValue(oper_symb), NULL, NULL);

		return 1; // MOVING CODE STR ON 1
	}
	else
    {
		return 0;
    }
}

size_t SetIdStr(const char* code, char* id_str)
{
	assert(code   != NULL);
	assert(id_str != NULL);

	size_t curr_ch_num = 0;

	if(!IsCharLetter(code[curr_ch_num]))
    {
		return 0;
    }

	while(IsCharLetter(code[curr_ch_num]) || isdigit(code[curr_ch_num]))
	{
		id_str[curr_ch_num] = code[curr_ch_num];

		curr_ch_num++;
	}

	return curr_ch_num;
}

size_t TrySetId(Tree* expr_tree, TreeNode** token_ptr, const char* code, NameTable* common_nametable, size_t* locals_counter)
{
	assert(expr_tree        != NULL);
	assert(token_ptr        != NULL);
	assert(code             != NULL);
	assert(common_nametable != NULL);
	assert(locals_counter   != NULL);

	char id_str[MAX_ID_SIZE]   = "";
	size_t curr_ch_num         = 0;

	curr_ch_num += SetIdStr(code + curr_ch_num, id_str);

	if(!curr_ch_num)
    {
		return 0;
    }

	if(IsOper(id_str))
	{
		*token_ptr = CreateOperNode(expr_tree, GetOperValue(id_str), NULL, NULL);

		if(GetOperValue(id_str) == FUNC)
        {
			(*locals_counter)++;
        }
	}
	else
	{
		NameTableElem* nametable_elem_ptr = NameTableFind(common_nametable, id_str);
		if(!nametable_elem_ptr)
        {
			nametable_elem_ptr = NameTableAdd(common_nametable, id_str, common_nametable->size);
        }

		*token_ptr = CreateIdNode(expr_tree, nametable_elem_ptr);
	}
	return curr_ch_num;
}

#define TRY_SET_ELEM_AND_MOVE_STR_P(try_set_func, ...)											       \
	do																								    \
	{																								     \
		old_ch_num = curr_ch_num;																	      \
		curr_ch_num += try_set_func(expr_tree, &tokens[token_count],  __VA_ARGS__);                        \
		if(old_ch_num != curr_ch_num)	                                                                    \
        {															                                         \
			token_count++;		                                                                           	  \
        }                                                                                                      \
	}while(0)

TreeNode** DoLexicalAnalisys(Tree* expr_tree, const char* file_name, NameTable* common_nametable, size_t* locals_counter)
{
	assert(expr_tree        != NULL);
	assert(file_name        != NULL);
	assert(common_nametable != NULL);

    FILE* file         = fopen(file_name, "rb");
	size_t file_size   = GetFileSize(file, SEEK_SET);
    char* code         = (char*)calloc(file_size + 1, sizeof(char));
    TreeNode** tokens  = (TreeNode**)calloc(file_size, sizeof(TreeNode*));
    assert(tokens != NULL);

    fread(code, sizeof(char), file_size, file);
    fclose(file);

	size_t curr_ch_num = 0;
	size_t old_ch_num  = 0;
	size_t token_count = 0;

	while(curr_ch_num < file_size)
	{
		if(code[curr_ch_num] == '\0')
        {
			break;
        }
        


		TRY_SET_ELEM_AND_MOVE_STR_P(TrySetNum, code + curr_ch_num);
		TRY_SET_ELEM_AND_MOVE_STR_P(TrySetOperSymb, code + curr_ch_num);
        while(code[curr_ch_num] == ' ')
		{
			curr_ch_num++;
            continue;
		}
		TRY_SET_ELEM_AND_MOVE_STR_P(TrySetId, code + curr_ch_num, common_nametable, locals_counter);
	}

	TreeNode** tmp_tokens = (TreeNode**)realloc(tokens, (token_count+1) * sizeof(TreeNode*));
	if(!tmp_tokens)
    {
		return NULL;
    }

	tokens = tmp_tokens;

	free(code);

	return tokens;
}