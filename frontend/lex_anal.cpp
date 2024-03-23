#include "lex_anal.h"

int isdigit(char sym)
{
    return (sym >= '0' && sym <='9' ) ? 1 : 0;
}

int TrySetNum(Tree* expr_tree, TreeNode** token_ptr, char* code)
{
    assert(expr_tree != NULL);
    assert(code      != NULL);

    int curr_ch_num                  = 0;
    double num                          = 0;
    int int_part_of_num                 = 1;
    int fraction_part_digits_counter = 10;

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

int TrySetOperSymb(Tree* expr_tree, TreeNode** token_ptr, const char* code)
{
    assert(expr_tree != NULL);
    assert(token_ptr != NULL);
    assert(code      != NULL);

    char oper_symb[MAX_ID_SIZE] = "";
    oper_symb[0] = code[0];

    if(IsOper(oper_symb) && !IsCharLetter(oper_symb[0]))
    {
        *token_ptr = CreateOperNode(expr_tree, GetOperValue(oper_symb), NULL, NULL);

        return 1;
    }
    else
    {
        return 0;
    }
}

int SetIdStr(const char* code, char* id_str)
{
    assert(code   != NULL);
    assert(id_str != NULL);

    int curr_ch_num = 0;

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

int TrySetId(Tree* expr_tree, TreeNode** token_ptr, const char* code, NameTable* common_nametable, int* locals_counter)
{
    assert(expr_tree        != NULL);
    assert(token_ptr        != NULL);
    assert(code             != NULL);
    assert(common_nametable != NULL);
    assert(locals_counter   != NULL);

    char id_str[MAX_ID_SIZE]   = "";
    int curr_ch_num         = 0;

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

#define TRY_SET_ELEM_AND_MOVE_STR_P(try_set_func, ...)                                  \
    do                                                                                   \
    {                                                                                     \
        old_ch_num = curr_ch_num;                                                          \
        curr_ch_num += try_set_func(expr_tree, &tokens[token_count],  __VA_ARGS__);         \
        if(old_ch_num != curr_ch_num)                                                        \
        {                                                                                     \
            token_count++;                                                                     \
        }                                                                                       \
    }while(0)

TreeNode** DoLexicalAnalisys(Tree* expr_tree, const char* file_name, NameTable* common_nametable, int* locals_counter)
{
    assert(expr_tree        != NULL);
    assert(file_name        != NULL);
    assert(common_nametable != NULL);

    FILE* file         = fopen(file_name, "rb");
    int file_size      = GetFileSize(file, SEEK_SET);
    char* code         = (char*)calloc(file_size + 1, sizeof(char));
    TreeNode** tokens  = (TreeNode**)calloc(file_size, sizeof(TreeNode*));
    assert(tokens != NULL);

    fread(code, sizeof(char), file_size, file);
    fclose(file);

    int curr_ch_num = 0;
    int old_ch_num  = 0;
    int token_count = 0;

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