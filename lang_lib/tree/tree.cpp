#include "tree.h"

const char* leafcolor = "#5ebfff";
const char* nodecolor = "#63e69e";
const char* leftcolor = "#3cf71b";
const char* rightcolor = "#ff0000";

#define CASE(OP)                                             \
    case OP:                                                  \
        fprintf(dump_file, "%s", #OP);                         \
        break;

void TreeCtor(Tree* tree )
{
    assert( tree );

    tree->log = fopen( "tree/treelog.txt","wb" );
    tree->root = NULL;
    tree->size = 0;
}

void TreeDtor( Tree* tree)
{
    if( tree == NULL )
        return;

    NodeDtor(tree->root);

    tree->root = NULL;
    tree->size = DEAD_SIZE;
    fclose( tree->log );
}

int NodeDtor( Node* node)
{
    if( node == NULL )
        return 1;
    
    if( node->left  != NULL ) 
        NodeDtor( node->left );
    if( node->right != NULL ) 
        NodeDtor( node->right );

    node->left  = NULL;
    node->right = NULL;
    node->value = NULL;
    free(node);
}

Node* CopyNode( Node* data )
{
	if( !data )
		return NULL;

    Node* node = ( Node* )calloc( 1, sizeof( Node ) );

    node->value = ( value* )calloc( 1, sizeof( value ) );
	node->value->arg = data->value->arg;
    node->value->type = data->value->type;
    node->parent = NULL;
	if(data->left)
    {
	    node->left  = CopyNode( data->left);
        node->left->parent = node;
    }
    if(data->right)
    {
	    node->right = CopyNode( data->right);
        node->right->parent = node;
    }
    
	return node;
}

int InviteNode( Tree* tree, Node* node, WhichChild child, value_t value )
{
    assert( tree );

    if( node == NULL && node != tree->root )
        return NODE_PTR_IS_NULL;

    Node* new_node = ( Node* )calloc( 1, sizeof( Node ) );
    if( new_node == NULL )
        return NODE_PTR_IS_NULL;

    new_node->value = value;

    if( node == tree->root && tree->size == 0 )
    {
        tree->root = new_node;
        tree->size++;
        return TREE_IS_OK;
    }

    if( child == Left && node->left == NULL )
        node->left = new_node;
    else if( child == Right && node->right == NULL )
        node->right = new_node;
    else
        return ADD_NODE_ERROR; 

    tree->size++;
    return TREE_IS_OK;
}

int RemoveNode( Tree* tree, Node* node )
{
    assert( tree );

    if( node == NULL )
        return 1;

    if( node->left == NULL && node->right == NULL )
    {
        if( node == tree->root ) 
            return TRY_REMOVE_ROOT;
        node->value = NULL;
        free(node);
        node = NULL;
        return 0;
    }
    else
        return TRY_REMOVE_NONLEAF;
}

int NodesVerify( Node* node )
{    
    int status = TREE_IS_OK;

    if( node == NULL )
    {
        status |= NODE_PTR_IS_NULL; 
        return status;
    }

    if ( node->left  != NULL ) 
        status |= NodesVerify(node->left);
    if (node->right != nullptr)
        status |= NodesVerify(node->right);

    return status;
}

int TreeVerify( Tree* tree )
{
    if ( tree == NULL )
        return TREE_IS_NULL;

    if ( tree->root == NULL )
        return TREE_ROOT_IS_NULL;

    int status = TREE_IS_OK;
    if (tree->root->left  != NULL)
        status |= NodesVerify(tree->root->left);

    if (tree->root->right != NULL)
        status |= NodesVerify(tree->root->right);

    tree->errors = status;
    return tree->errors;
}

int NodeDump( Node *node, size_t number_of_node, FILE *dump_file )
{
    if( node )
    {
        if ( node->left == NULL && node->right == NULL )
            fprintf(dump_file, "    node%lu [fillcolor=\"%s\", ", number_of_node, leafcolor);
        else
            fprintf(dump_file, "    node%lu [fillcolor=\"%s\", ", number_of_node, nodecolor);
        fprintf(dump_file, "label=\"");
        
        if (node->value == nullptr)
        {
            fprintf(dump_file, "empty");
        }
        else if (node->value->type == OPER_TYPE)
        {
            switch ( (char) node->value->arg )
            {
                case MUL:
                    fprintf(dump_file, "%c", (char) node->value->arg);
                    break;

                case SUB:
                    fprintf(dump_file, "%c", (char) node->value->arg);
                    break;

                case ADD:
                    fprintf(dump_file, "%c", (char) node->value->arg);
                    break;
            
                case DIV:
                    fprintf(dump_file, "%c", (char) node->value->arg);
                    break;

                case POW:
                    fprintf(dump_file, "%c", (char) node->value->arg);
                    break;
                
                case EXP:
                    fprintf(dump_file, "e");
                    break;
            
                case PI:
                   fprintf(dump_file, "pi");
                    break;
            
                CASE(SH);
                CASE(CH);
                CASE(SIN);
                CASE(COS);
                CASE(LN);

                default:
                    fprintf(dump_file, "dead");
                    break;
            }
        }
        else if (node->value->type == NUM_TYPE)
        {
            fprintf(dump_file, "%lg", node->value->arg);
        }
        else if (node->value->type == VAR_TYPE)
        {
            fprintf(dump_file, "%c", (char) node->value->arg);
        }
        fprintf(dump_file, "\"");
        fprintf(dump_file, "];\n");
    
        if (node->left)
        {
            fprintf(dump_file, "    edge [color=\"%s\"]; node%lu -> node%lu;\n", leftcolor, number_of_node, number_of_node * 2 + 1 );
            NodeDump(node->left, number_of_node * 2 + 1, dump_file);
        }
        if (node->right)
        {
            fprintf(dump_file, "    edge [color=\"%s\"]; node%lu -> node%lu;\n", rightcolor, number_of_node, number_of_node * 2 + 2 );
            NodeDump(node->right, number_of_node * 2 + 2, dump_file);
        }
    }
}

int TreeDump( Tree* tree, FILE* logger )
{
    assert( tree );

    int status = TreeVerify( tree );

    static size_t num_of_call = 1;

	fprintf( logger, "=======================================\nTREE DUMP CALL #%zu\nSize of tree: %zu\n", num_of_call, tree->size );
    if ( tree->errors )
	{
		fprintf( logger, "-------------ERRORS------------\n" );
		if ( tree->errors & TREE_IS_NULL )
		{
			fprintf( logger, "TREE POINTER IS NULL\n" );
			return 0;
		}
        if ( tree->errors & TREE_LOGER_ERROR ) fprintf( logger, "TREE LOGGER ERROR\n" );
		if ( tree->errors & TREE_ROOT_IS_NULL ) fprintf( logger, "ROOT OF TREE POINTER IS NULL\n" );
		if ( tree->errors & NODE_PTR_IS_NULL ) fprintf( logger, "TRY TO WORK WITH NULL NODE\n" );
	    if ( tree->errors & TRY_REMOVE_ROOT ) fprintf( logger, "TRY TO REMOVE ROOT NODE\n");
	    if ( tree->errors & TRY_REMOVE_NONLEAF ) fprintf( logger, "TRY TO REMOVE NON LEAF NODE \n" );
        if ( tree->errors & ADD_NODE_ERROR ) fprintf( logger, "THIS LEAF IS ALREADY EXIST \n" );

		fprintf( logger, "----------END_OF_ERRORS--------\n" );
	}
	else
    {
		fprintf( logger, "------------NO_ERRORS----------\n" );
        size_t number_of_node = 0;
        FILE* dump_file = fopen( "tree/dump.gv", "w" );
    
        fputs( "digraph structs {\n", dump_file );
        fputs( "    node [color=black, shape=box, style=\"filled\"];\n", dump_file );

        status = NodeDump( tree->root, number_of_node, dump_file );

        fputs( "}\n", dump_file );

        fclose( dump_file );
    }
	fprintf( logger, "=======================================\n\n" );
	num_of_call++;

    return status;
}
