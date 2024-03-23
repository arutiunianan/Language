#include "syn_tree.h"

int REC_INDENT_SIZE = 2;
void WriteNodePreOrder(Tree* tree, const TreeNode* node, FILE* syn_file, int rec_depth)
{
    assert(tree     != NULL);
    assert(syn_file != NULL);

    if(!node) 
    { 
        fprintf(syn_file, "%*s" EMPTY_NODE "\n", rec_depth * REC_INDENT_SIZE, ""); 
        return; 
    }

    fprintf(syn_file, "%*s(", rec_depth * REC_INDENT_SIZE, "");

    fprintf(syn_file, "%s\n", tree->ElemPrinter(&node->node_elem));
    WriteNodePreOrder(tree, node->left,  syn_file, rec_depth + 1);
    WriteNodePreOrder(tree, node->right, syn_file, rec_depth + 1);
    
    fprintf(syn_file, "%*s)\n", rec_depth * REC_INDENT_SIZE, "");
}

void WriteSYNTree(Tree* tree, FILE* logger)
{
    assert(tree   != NULL);
    assert(logger != NULL);

    fprintf(logger, "Tree: {\n");
    WriteNodePreOrder(tree, tree->root, logger, 1);
    fprintf(logger, "}");
}

const double EPSILON = 10e-15;
int IsDoubleInt(double num)
{
    return num - (int)num < EPSILON;
}

char* SYNTreeElemPrinter(const TreeNode_t* elem_to_print)
{
    assert(elem_to_print != NULL);

    static char str_to_output[MAX_ID_SIZE] = "";

    if(elem_to_print->type == NUM)
    {
        if(IsDoubleInt(elem_to_print->elem.num))
        {
            sprintf(str_to_output, "%d", (int)elem_to_print->elem.num);
        }
        else
        {
            sprintf(str_to_output, "%lf", elem_to_print->elem.num);
        }
    }
    else if(elem_to_print->type == ID)
    {
        if(!elem_to_print->elem.id)
        {
            sprintf(str_to_output, "%s", "Elem Var PIZDEC NULL");
        }
        else
        {
            sprintf(str_to_output, "%s type:%d", elem_to_print->elem.id->name, elem_to_print->elem.id->type);
        }
    }
    else if(elem_to_print->type == OPER)
    {
        sprintf(str_to_output, "%s", GetOperDesignation(elem_to_print->elem.oper));
    }
    else
    {
        assert(0);
    }
    
    return str_to_output;
}


char* GetOperDesignation(Operation oper)
{
    switch(oper)
    {
        #define OPER_DEF(value, designation1, designation2)     \
        case value:                                              \
            return designation2;

        #include "../oper_defs.h"
        #undef OPER_DEF
    }
}

Operation GetOperValue(const char* design)
{
    assert(design != NULL);

    Operation oper = WRONG_OPER;

    #define OPER_DEF(value, designation1, designation2) \
        if(strcmp(design, designation1) == 0)            \
        {                                                 \
            return value;                                  \
        }                                                   \
        if(strcmp(design, designation2) == 0)                \
        {                                                     \
            return value;                                      \
        }

    #include "../oper_defs.h"

    #undef OPER_DEF

    return WRONG_OPER;
}

TreeNode* GetProperNode(Tree* tree, ProgramNameTables* nametables, int* current_local, FILE* syn_file)
{
	assert(tree          != NULL);
	assert(nametables    != NULL);
	assert(current_local != NULL);
	assert(syn_file      != NULL);

	double num_node_arg            = 0;
	char str_node_arg[MAX_ID_SIZE] = "";
	int file_cursor_pos            = 0;

	int read_args_num = 0;	
	
	file_cursor_pos = ftell(syn_file);
	read_args_num   = fscanf(syn_file, " %lf", &num_node_arg);
	
	if(!read_args_num)
	{	
		fseek(syn_file, file_cursor_pos, SEEK_SET);

		read_args_num = fscanf(syn_file, " %s", str_node_arg);
		
		if(!read_args_num)
		{
			return NULL;
		}

		if(IsOper(str_node_arg))
		{
			if(GetOperValue(str_node_arg) == FUNC)
			{
				(*current_local)++;
			}

			return CreateOperNode(tree, GetOperValue(str_node_arg), NULL, NULL);
		}
		else
		{
			TreeNode*         id_node = NULL;
			NameTableElemType id_type = error_type;
			read_args_num = fscanf(syn_file, " type:%d", &id_type);

			if(!read_args_num)
			{
				return NULL;
			}

			if(id_type == variable)
			{
				id_node = CreateIdNode(tree, NameTableFind(&nametables->local_nametables[*current_local], str_node_arg));			
			}
			else if(id_type == function)
			{
				id_node = CreateIdNode(tree, NameTableFind(&nametables->functions_nametable, str_node_arg));
			}
			else
			{
				assert(0);
			}

			id_node->node_elem.elem.id->type = id_type;
			
			return id_node;
		}
	}
	else
	{
		return CreateNumNode(tree, num_node_arg);
	}
}

#define SET_ERROR_AND_RET_IF_NEEDED(cond, error) \
    if(!(cond))                                   \
    {                                              \
        *errors |= error;                           \
        return;                                      \
    }                                                 \

void ReadNode(Tree* tree, TreeNode** node_ptr, ProgramNameTables* nametables, int* current_scope, FILE* syn_file, int* errors)
{
    assert(tree          != NULL);
    assert(node_ptr      != NULL);
    assert(syn_file      != NULL);
    assert(current_scope != NULL);
    assert(errors        != NULL);

    if(*errors)
	{
        return;
	}

    int read_args_num = 0;
    int cmd_len          = 0;
    
    read_args_num = fscanf(syn_file, " (%n", &cmd_len);

    if(!cmd_len)
    {
        char tmp_str[MAX_ID_SIZE] = "";
        read_args_num = fscanf(syn_file, " %s", tmp_str);            
        SET_ERROR_AND_RET_IF_NEEDED(strcmp(tmp_str, EMPTY_NODE) == 0, 1);

        return;
    }

    *node_ptr = GetProperNode(tree, nametables, current_scope, syn_file);
    SET_ERROR_AND_RET_IF_NEEDED(*node_ptr, 1);

    ReadNode(tree, &(*node_ptr)->left,  nametables, current_scope, syn_file, errors);
    ReadNode(tree, &(*node_ptr)->right, nametables, current_scope, syn_file, errors);

    read_args_num = fscanf(syn_file, " )%n", &cmd_len);
    SET_ERROR_AND_RET_IF_NEEDED(cmd_len, 1);
}

void ReadSYNTree(Tree* tree, ProgramNameTables* nametables, FILE* syn_file, int* errors)
{
    assert(tree     != NULL);
    assert(syn_file != NULL);

    int valid_args_num = 0;
    int cmd_len           = 0;

    valid_args_num = fscanf(syn_file, " Tree: {%n", &cmd_len);
    SET_ERROR_AND_RET_IF_NEEDED(cmd_len, 1);

    int current_scope = -1;
    ReadNode(tree, &tree->root, nametables, &current_scope, syn_file, errors);
    
    valid_args_num = fscanf(syn_file, " }%n", &cmd_len);
    SET_ERROR_AND_RET_IF_NEEDED(cmd_len, 1);
}


int IsOper(const char* design)
{
    assert(design != NULL);

    #define OPER_DEF(value, designation1, designation2) \
        if(strcmp(design, designation1) == 0)            \
        {                                                 \
            return 1;                                      \
        }                                                   \
        if(strcmp(design, designation2) == 0)                \
        {                                                     \
            return 1;                                          \
        }

    #include "../oper_defs.h"

    #undef OPER_DEF

    return 0;
}

TreeNode* CreateNumNode(Tree* expr_tree, double num)
{
    assert(expr_tree != NULL);

    TreeNode*  new_node = NULL;
    TreeNode_t new_data = {};

    new_data.type     = NUM;
    new_data.elem.num = num;

    return CreateNode(expr_tree, &new_data, &new_node, NULL, NULL);
}

TreeNode* CreateOperNode(Tree* expr_tree, Operation oper, TreeNode* left_node, TreeNode* right_node)
{
    assert(expr_tree != NULL);;

    TreeNode*  new_node = NULL;
    TreeNode_t new_data = {};

    new_data.type      = OPER;
    new_data.elem.oper = oper;

    return CreateNode(expr_tree, &new_data, &new_node, left_node, right_node);
}

TreeNode* CreateIdNode(Tree* expr_tree, NameTableElem* nametable_elem_ptr)
{
    assert(expr_tree != NULL);

    TreeNode*  new_node = NULL;
    TreeNode_t new_data = {};

    new_data.type    = ID;
    new_data.elem.id = nametable_elem_ptr;

    return CreateNode(expr_tree, &new_data, &new_node, NULL, NULL);
}

