#include "syn_tree.h"

int REC_INDENT_SIZE = 2;
void WriteNodePreOrder(Tree* tree, const TreeNode* node, FILE* ast_file, int rec_depth)
{
	assert(tree     != NULL);
	assert(ast_file != NULL);

	if(!node) 
    { 
        fprintf(ast_file, "%*s" EMPTY_NODE "\n", rec_depth * REC_INDENT_SIZE, ""); 
        return; 
    }

	fprintf(ast_file, "%*s(", rec_depth * REC_INDENT_SIZE, "");

	fprintf(ast_file, "%s\n", tree->ElemPrinter(&node->node_elem));
	WriteNodePreOrder(tree, node->left,  ast_file, rec_depth + 1);
	WriteNodePreOrder(tree, node->right, ast_file, rec_depth + 1);
	
	fprintf(ast_file, "%*s)\n", rec_depth * REC_INDENT_SIZE, "");
}

const double EPSILON = 10e-15;
bool IsDoubleInt(double num)
{
	return num - (int)num < EPSILON;
}

void WriteASTTree(Tree* tree, FILE* logger)
{
	assert(tree   != nullptr);
	assert(logger != nullptr);

	fprintf(logger, "Tree: {\n");
	WriteNodePreOrder(tree, tree->root, logger, 1);
	fprintf(logger, "}");
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

char* SYNTreeElemPrinter(const TreeNode_t* elem_to_print)
{
	assert(elem_to_print != nullptr);

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
