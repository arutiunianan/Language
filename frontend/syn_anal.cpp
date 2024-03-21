#include "syn_anal.h"

#define TOKEN_TYPE           tokens[*current_token_num]->node_elem.type
#define TOKEN_OPER           tokens[*current_token_num]->node_elem.elem.oper
#define TOKEN_NUM            tokens[*current_token_num]->node_elem.elem.num
#define TOKEN_ID_NAME        tokens[*current_token_num]->node_elem.elem.id->name
#define INC_TOKEN_NUM        (*current_token_num)++
#define DEC_TOKEN_NUM        (*current_token_num)--

#define GET_NUM(...)         GetN                  (tokens, current_token_num)
#define GET_ID(...)          GetId                 (tokens, current_token_num)
#define GET_EXPR(...)        GetExpr               (tokens, current_token_num, nametables)
#define GET_PRIMARY(...)     GetPrimary            (tokens, current_token_num, nametables)
#define GET_TERM(...)        GetTerm               (tokens, current_token_num, nametables)
#define GET_ASSIGN(...)      GetAssign             (tokens, current_token_num, nametables)
#define GET_FUNC_ARGS(...)   GetFunctionArgs       (tokens, current_token_num, nametables, &args_count)
#define GET_FUNC_CALL(...)   GetFunctionCall       (tokens, current_token_num, nametables)
#define GET_IF(...)          GetIf                 (tokens, current_token_num, nametables)
#define GET_WHILE(...)       GetWhile              (tokens, current_token_num, nametables)
#define GET_OPER(...)        GetOper               (tokens, current_token_num, nametables)
#define GET_USER_INPUT(...)  GetUserInput          (tokens, current_token_num, nametables)
#define GET_USER_OUTPUT(...) GetUserOutput         (tokens, current_token_num, nametables)
#define GET_RETURN(...)      GetReturn             (tokens, current_token_num, nametables)
#define GET_LOGIC(...)       GetLogic              (tokens, current_token_num, nametables)
#define GET_POW(...)         GetPow                (tokens, current_token_num, nametables)

void PrintCurrentToken(TreeNode** tokens, int* current_token_num)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);

    switch(TOKEN_TYPE)
    {
        case ID:
            printf("%s\n", TOKEN_ID_NAME);
            break;
        case NUM:
            printf("%lf\n", TOKEN_NUM);
            break;
        case OPER:
            printf("%s\n", GetOperDesignation(TOKEN_OPER));
            break;
        default:
            assert(0);
    }
}

#define SYN_ASSERT(condition)                           \
    if(!(condition))                                     \
    {                                                     \
        printf("Token TYPE: %d\n", TOKEN_TYPE);            \
        PrintCurrentToken(tokens, current_token_num);       \
        abort();                                             \
    }


TreeNode* GetLogic(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables);
TreeNode* GetExpr (TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables);
TreeNode* GetOper (TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables);

TreeNode* GetN(TreeNode** tokens, int* current_token_num)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);

    if(TOKEN_TYPE == NUM)
    {
        INC_TOKEN_NUM;
        return tokens[*current_token_num-1];
    }

    return NULL;
}

TreeNode* GetId(TreeNode** tokens,  int* current_token_num)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);

    if(tokens[*current_token_num]->node_elem.type == ID)
    {
        INC_TOKEN_NUM;
        return tokens[*current_token_num-1];
    }

    return NULL;
}

TreeNode* GetFunctionCall(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables);

TreeNode* GetPrimary(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);    
    
    if(TOKEN_TYPE == OPER && TOKEN_OPER == OBR)
    {
        INC_TOKEN_NUM;
        TreeNode* logic_node = GET_LOGIC();
        
        SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == CBR);
        INC_TOKEN_NUM;
        
        SYN_ASSERT(logic_node != NULL)

        return logic_node;
    }
    else if(TOKEN_TYPE == NUM)
    {
        TreeNode* num_node = GET_NUM();
    
        SYN_ASSERT(num_node != NULL);
        return num_node;
    }
    else if(TOKEN_TYPE == ID)
    {
        TreeNode* id_node = GET_FUNC_CALL();
        
        if(!id_node)
        {
            id_node = GET_ID();
            SYN_ASSERT(NameTableFind(&CURRENT_LOCAL_NAMETABLE, id_node->node_elem.elem.id->name));
        }

        return id_node;
    }
    else
    {
        SYN_ASSERT(0);
    }
}

TreeNode* GetPow(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);    
    
    TreeNode* primary_node1 = GET_PRIMARY();
    
    while(TOKEN_OPER == POW)
    {
        int oper_token_num = *current_token_num;

        INC_TOKEN_NUM;

        TreeNode* primary_node2 = GET_PRIMARY();
        
        tokens[oper_token_num]->left  = primary_node1;
        tokens[oper_token_num]->right = primary_node2;    

        primary_node1 = tokens[oper_token_num];
    }                                                                                                                                                                                                                                                
                                                                                                                                                                                                                                                                                                                                                                        
    return primary_node1;
}

TreeNode* GetTerm(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);    
    
    TreeNode* primary_node1 = GET_POW();
    
    while(TOKEN_OPER == MUL || TOKEN_OPER == DIV)
    {
        int oper_token_num = *current_token_num;

        INC_TOKEN_NUM;

        TreeNode* primary_node2 = GET_POW();
        
        tokens[oper_token_num]->left  = primary_node1;
        tokens[oper_token_num]->right = primary_node2;    

        primary_node1 = tokens[oper_token_num];
    }                                                                                                                                                                                                                                                                                                                                                                    
                                                                                                                                                                                                                                                                                                                                                                        
    return primary_node1;
}

TreeNode* GetExpr(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);    
    
    TreeNode* term_node1 = GET_TERM();
    
    while(TOKEN_OPER == ADD || TOKEN_OPER == SUB)
    {
        int oper_token_num = *current_token_num;

        INC_TOKEN_NUM;

        TreeNode* term_node2 = GET_TERM();
        
        tokens[oper_token_num]->left  = term_node1;
        tokens[oper_token_num]->right = term_node2;    

        term_node1 = tokens[oper_token_num];
    }

    return term_node1;
}

TreeNode* GetLogic(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);    
    
    TreeNode* expr_node1 = GET_EXPR();
    
    while(TOKEN_OPER == LG || TOKEN_OPER == LE || TOKEN_OPER == LNE || 
		  TOKEN_OPER == LL || TOKEN_OPER == AND || TOKEN_OPER == OR)
    {
        int oper_token_num = *current_token_num;

        INC_TOKEN_NUM;

        TreeNode* expr_node2 = GET_EXPR();
        
        tokens[oper_token_num]->left  = expr_node1;
        tokens[oper_token_num]->right = expr_node2;    

        expr_node1 = tokens[oper_token_num];
    }

    return expr_node1;
}

TreeNode* GetAssign(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);

    TreeNode* var_name_node = GET_ID();
    if(!var_name_node)
    {
        return NULL;
    }

    if(!(TOKEN_TYPE == OPER && TOKEN_OPER == ASSIGN))
    {
        return NULL;
    }

    int assign_token_num = *current_token_num;
    INC_TOKEN_NUM;

    char* var_name = var_name_node->node_elem.elem.id->name;
    var_name_node->node_elem.elem.id = ProgramNameTablesAddVar(nametables, var_name);
    
    tokens[assign_token_num]->left  = var_name_node;
    tokens[assign_token_num]->right = GET_LOGIC();

    return tokens[assign_token_num];
}

TreeNode* GetFunctionArgs(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables, int* args_count)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);

    TreeNode* func_args_node1 = GET_ID();

    if(!func_args_node1)
    {
        return NULL;
    }

    (*args_count)++;

    if(func_args_node1 && (TOKEN_TYPE == OPER && TOKEN_OPER == FUNC_SEP))
    {
        int sep_token_num = *current_token_num;
        INC_TOKEN_NUM;

        TreeNode* func_args_node2 = NULL;

        while(func_args_node2 = GET_ID())
        {
            (*args_count)++;

            SYN_ASSERT(func_args_node2);

            tokens[sep_token_num]->left  = func_args_node1;
            tokens[sep_token_num]->right = func_args_node2;

            func_args_node1 = tokens[sep_token_num];

            if(TOKEN_TYPE == OPER && TOKEN_OPER == FUNC_SEP)
            {
                sep_token_num = *current_token_num;
                INC_TOKEN_NUM;
                continue;
            }
            else
            {
                break;
            }
        }
    }
    
    return func_args_node1;
}

TreeNode* GetFunctionCall(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);    
    
    TreeNode* func_name_node = GET_ID();
    if(!func_name_node)
    {
        return NULL;
    }

    if(!(TOKEN_TYPE == OPER && TOKEN_OPER == OBR))
    {
        DEC_TOKEN_NUM;
        return NULL;
    }
    else
    {
        INC_TOKEN_NUM;
    }

    char* func_name = func_name_node->node_elem.elem.id->name;
    SYN_ASSERT(func_name_node->node_elem.elem.id = NameTableFind(&nametables->functions_nametable, func_name));

    int args_count       = 0;
    func_name_node->left = GET_FUNC_ARGS(&args_count);
    SYN_ASSERT(args_count == func_name_node->node_elem.elem.id->num);

    SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == CBR);
    INC_TOKEN_NUM;

    return func_name_node;
}

TreeNode* GetIf(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);

    if(!(TOKEN_TYPE == OPER && TOKEN_OPER == IF))
    {
        return NULL;
    }

    int if_token_num = *current_token_num;

    INC_TOKEN_NUM;
    
    SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == OBR);
    INC_TOKEN_NUM;
    
    TreeNode* logic_node = GET_LOGIC();
    
    SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == CBR);
    INC_TOKEN_NUM;

    TreeNode* oper_node = GET_OPER();

    SYN_ASSERT(oper_node != NULL && logic_node != NULL);

    tokens[if_token_num]->left  = logic_node;
    tokens[if_token_num]->right = oper_node;

    return tokens[if_token_num];
}

TreeNode* GetWhile(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);    
    
    if(!(TOKEN_TYPE == OPER && TOKEN_OPER == WHILE))
    {
        return NULL;
    }

    int while_token_num = *current_token_num;

    INC_TOKEN_NUM;

    SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == OBR);
    INC_TOKEN_NUM;
    
    TreeNode* logic_node = GET_LOGIC();
    
    SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == CBR);
    INC_TOKEN_NUM;

    TreeNode* oper_node = GET_OPER();

    SYN_ASSERT(oper_node != NULL && logic_node != NULL);

    tokens[while_token_num]->left  = logic_node;
    tokens[while_token_num]->right = oper_node;

    return tokens[while_token_num];
}

TreeNode* GetUserInput(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);

    if(!(TOKEN_TYPE == OPER && TOKEN_OPER == USER_INPUT))
    {
        return NULL;
    }

    int user_input_token_num = *current_token_num;

    INC_TOKEN_NUM;

    SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == OBR);
    INC_TOKEN_NUM;

    TreeNode* id_node = GET_ID();
    SYN_ASSERT(NameTableFind(&CURRENT_LOCAL_NAMETABLE, id_node->node_elem.elem.id->name));

    SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == CBR);
    INC_TOKEN_NUM;

    tokens[user_input_token_num]->left = id_node;

    return tokens[user_input_token_num];
}

TreeNode* GetUserOutput(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);

    if(!(TOKEN_TYPE == OPER && TOKEN_OPER == USER_OUTPUT) && !(TOKEN_TYPE == OPER && TOKEN_OPER == USER_OUTPUT))
    {
        return NULL;
    }

    int user_output_token_num = *current_token_num;

    INC_TOKEN_NUM;

    SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == OBR);
    INC_TOKEN_NUM;

    TreeNode* id_node = GET_ID();
    SYN_ASSERT(NameTableFind(&CURRENT_LOCAL_NAMETABLE, id_node->node_elem.elem.id->name));

    SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == CBR);
    INC_TOKEN_NUM;

    tokens[user_output_token_num]->left = id_node;

    return tokens[user_output_token_num];
}

TreeNode* GetReturn(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);

    if(!(TOKEN_TYPE == OPER && TOKEN_OPER == RETURN))
    {
        return NULL;
    }

    int return_token_num = *current_token_num;

    INC_TOKEN_NUM;

    TreeNode* return_obj_node = GET_ID();
    if(!return_obj_node)
    {
        return_obj_node = GET_NUM();
        SYN_ASSERT(return_obj_node);
    }
    else
    {
        SYN_ASSERT(NameTableFind(&CURRENT_LOCAL_NAMETABLE, return_obj_node->node_elem.elem.id->name));
    }

    tokens[return_token_num]->left = return_obj_node;

    return tokens[return_token_num];
}

TreeNode* GetOper(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);    
    
    TreeNode* return_node = GET_RETURN();
    if(return_node)
    {
        return return_node;
    }

    TreeNode* user_output_node = GET_USER_OUTPUT();
    if(user_output_node)
    {
        return user_output_node;
    }

    TreeNode* user_input_node = GET_USER_INPUT();
    if(user_input_node)
    {
        return user_input_node;
    }

    TreeNode* if_node = GET_IF();
    if(if_node)
    {
        return if_node;
    }

    TreeNode* while_node = GET_WHILE();
    if(while_node)
    {
        return while_node;
    }

    TreeNode* func_call_node = GET_FUNC_CALL();
    if(func_call_node)
    {
        SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == TAB);
        return func_call_node;
    }

    TreeNode* assign_node = GET_ASSIGN();
    if(assign_node)
    {
        SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == TAB);
        return assign_node;
    }

    if(TOKEN_TYPE == OPER && TOKEN_OPER == TAB)
    {
        INC_TOKEN_NUM;
        TreeNode* oper_node1 = GET_OPER();
        TreeNode* oper_node2 = NULL;

        SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == TAB);
        INC_TOKEN_NUM;

        while(oper_node2 = GET_OPER())
        {
            int sep_token_num = *current_token_num;
            SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == TAB);

            tokens[sep_token_num]->left  = oper_node1;
            tokens[sep_token_num]->right = oper_node2;

            oper_node1 = tokens[sep_token_num];

            INC_TOKEN_NUM;
        }
    
        return oper_node1;
    }

    return NULL;
}

TreeNode* GetFunctionParams(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables, int* params_count)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);

    TreeNode* func_param_node1 = GET_ID();

    if(!func_param_node1)
    {
        return NULL;
    }

    char* func_param_name1 = func_param_node1->node_elem.elem.id->name;
    func_param_node1->node_elem.elem.id = ProgramNameTablesAddVar(nametables, func_param_name1);
    
    (*params_count)++;

    if(func_param_node1 && (TOKEN_TYPE == OPER && TOKEN_OPER == FUNC_SEP))
    {
        int sep_token_num = *current_token_num;
        INC_TOKEN_NUM;

        TreeNode* func_param_node2 = NULL;

        while(func_param_node2 = GET_ID())
        {
            SYN_ASSERT(func_param_node2);
            (*params_count)++;

            char* func_param_name2 = func_param_node2->node_elem.elem.id->name;
            func_param_node2->node_elem.elem.id = ProgramNameTablesAddVar(nametables, func_param_name2);

            tokens[sep_token_num]->left  = func_param_node1;
            tokens[sep_token_num]->right = func_param_node2;

            func_param_node1 = tokens[sep_token_num];

            if(TOKEN_TYPE == OPER && TOKEN_OPER == FUNC_SEP)
            {
                sep_token_num = *current_token_num;
                INC_TOKEN_NUM;
                continue;
            }
            else
            {
                break;
            }
        }
    }

    return func_param_node1;
}

TreeNode* GetFunctionDefinition(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);

    if(!tokens[*current_token_num])
    {
        return NULL;
    }

    if(!(TOKEN_TYPE == OPER && TOKEN_OPER == FUNC))
    {
        return NULL;
    }

    int func_token_num = *current_token_num;
    INC_TOKEN_NUM;

    TreeNode* func_name_node = GET_ID();

    char* func_name = func_name_node->node_elem.elem.id->name;
    SYN_ASSERT(func_name_node->node_elem.elem.id = ProgramNameTablesAddFunc(nametables, func_name));

    SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == OBR);
    INC_TOKEN_NUM;

    int params_count = 0;
    TreeNode* func_param_node = GetFunctionParams(tokens, current_token_num, nametables, &params_count);
    func_name_node->node_elem.elem.id->num = params_count;

    SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == CBR);
    INC_TOKEN_NUM;

    TreeNode* oper_node = GET_OPER();

    tokens[func_token_num]->left       = func_name_node;
    tokens[func_token_num]->left->left = func_param_node; 
    tokens[func_token_num]->right      = oper_node;

    return tokens[func_token_num];
}

TreeNode* GetProgram(TreeNode** tokens, int* current_token_num, ProgramNameTables* nametables)
{
    assert(tokens            != NULL);
    assert(current_token_num != NULL);
    assert(nametables        != NULL);

    TreeNode* func_node1 = GetFunctionDefinition(tokens, current_token_num, nametables);
    TreeNode* func_node2 = NULL;
    
    SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == TAB);
    INC_TOKEN_NUM;

    while(func_node2 = GetFunctionDefinition(tokens, current_token_num, nametables))
    {
        int sep_token_num = *current_token_num;
        SYN_ASSERT(TOKEN_TYPE == OPER && TOKEN_OPER == TAB);

        tokens[sep_token_num]->left  = func_node1;
        tokens[sep_token_num]->right = func_node2;

        func_node1 = tokens[sep_token_num];

        INC_TOKEN_NUM;
    } 

    return func_node1;
}












Tree GetCodeTree(const char* file_name, ProgramNameTables* nametables)
{ 
    assert(file_name  != NULL);
    assert(nametables != NULL);

    TreeNode** tokens = NULL;
    Tree expr_tree    = {};
    TreeCtor(&expr_tree, SYNTreeElemPrinter);

    int locals_counter = 0;
    tokens = DoLexicalAnalisys(&expr_tree, file_name, &nametables->common_nametable, &locals_counter);

    ProgramNameTablesCtor(nametables, locals_counter);

    int current_token_num = 0;
    expr_tree.root = GetProgram(tokens, &current_token_num, nametables);

    return expr_tree;
}

#undef TOKEN_TYPE            
#undef TOKEN_OPER            
#undef TOKEN_NUM            
#undef TOKEN_ID_STR            
#undef INC_TOKEN_NUM        
#undef DEC_TOKEN_NUM        

#undef GET_NUM     
#undef GET_ID         
#undef GET_EXPR       
#undef GET_PRIMARY   
#undef GET_TERM       
#undef GET_ASSIGN    
#undef GET_FUNC_ARGS  
#undef GET_FUNC_CALL  
#undef GET_IF         
#undef GET_WHILE      
#undef GET_OPER       
#undef GET_FUNC_DEF 