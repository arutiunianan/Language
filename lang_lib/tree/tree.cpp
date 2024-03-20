#include "tree.h"

const char* num_color   = "#5ebfff";
const char* oper_color  = "#63e69e";
const char* var_color   = "#3cf71b";
const char* links_color = "#ff0000";
const char* font_color  = "#0e0a2a";

#define CHECK_ERROR(obj, condition, error)                          \
do                                                                   \
{                                                                     \
    if(condition)                                                      \
        (obj)->errors |= error;                                         \
    else                                                                 \
        (obj)->errors &= ~(error);                                        \
} while(0)

#define ERROR_PROCESSING(structure, StructVerifier, StructDump, StructDtor)  \
do                                                                            \
{                                                                              \
    if(StructVerifier(structure))                                              \
    {                                                                            \
        StructDump(structure, stderr);                                            \
        StructDtor(structure);                                                     \
        return (structure)->errors;                                                 \
    }                                                                                \
} while(0)


TreeNode* OpNew(TreeNode_t* data)
{
    assert(data != NULL);

    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if(node) 
    {
        node->node_elem = *data;
    }

    return node;
}

void OpDelete(TreeNode* node)
{
    assert(node != NULL);

    node->node_elem = {};
    free(node->left);
    free(node->right);
}

void PrintNodeInOrder(Tree* tree, const TreeNode* node, FILE* logger)
{
    assert(tree   != NULL);
    assert(logger != NULL);

    if(!node) 
    { 
        fprintf(logger, " " EMPTY_NODE); 
        return; 
    }
    
    fprintf(logger, "( ");
    
    PrintNodeInOrder(tree, node->left,  logger);
    fprintf(logger, "%s ", tree->ElemPrinter(&node->node_elem));
    PrintNodeInOrder(tree, node->right, logger);

    fprintf(logger, ") ");
}

void PrintNodePostOrder(Tree* tree, const TreeNode* node, FILE* logger)
{
    assert(tree   != NULL);
    assert(logger != NULL);

    if(!node) 
    { fprintf(logger, " " EMPTY_NODE); return; }
    
    fprintf(logger, "( ");
    
    PrintNodePostOrder(tree, node->left,  logger);
    PrintNodePostOrder(tree, node->right, logger);
    fprintf(logger, "%s ", tree->ElemPrinter(&node->node_elem));

    fprintf(logger, ") ");
}

void PrintNodePreOrder(Tree* tree, const TreeNode* node, FILE* logger)
{
    assert(tree   != NULL);
    assert(logger != NULL);

    if(!node) 
    { 
        fprintf(logger, " " EMPTY_NODE); 
        return; 
    }
    
    fprintf(logger, "( ");
    
    fprintf(logger, "%s ", tree->ElemPrinter(&node->node_elem));
    PrintNodePreOrder(tree, node->left,  logger);
    PrintNodePreOrder(tree, node->right, logger);

    fprintf(logger, ") ");
}

TreeNode* CopyNode(Tree* tree, TreeNode* node)
{
    assert(tree != NULL);

    if(!node)
    {
        return NULL;
    }

    TreeNode* new_node = {};
    new_node = OpNew(&node->node_elem);
    
    new_node->left  = CopyNode(tree, node->left);
    new_node->right = CopyNode(tree, node->right);

    return new_node;
}

void DestroyNode(Tree* tree, TreeNode* node)
{
    assert(tree != NULL);

    if(!node)
    {
        return;
    }

    if(node->left)
    {
        DestroyNode(tree, node->left);
    }
    if(node->right)
    {
        DestroyNode(tree, node->right);
    }

    OpDelete(node);
}

TreeNode* CreateNode(Tree* tree, TreeNode_t* data, TreeNode** node, TreeNode* left_node, TreeNode* right_node)
{
    assert(tree != NULL);
    assert(node != NULL);

    DestroyNode(tree, *node);

    *node = OpNew(data);

    (*node)->left  = left_node;
    (*node)->right = right_node;

    return *node;
}

void PrintGraphNode (Tree* tree, TreeNode* node, FILE* graph)
{
    assert(tree  != NULL);
    assert(graph != NULL);

    if(!node)
        return;

    if(node->node_elem.type == ID)
    {
        fprintf(graph, 
                "node [shape=\"box\", style=\"filled\", fillcolor=\"%s\", fontcolor=\"%s\", margin=\"0.01\"];\n", 
                var_color, font_color);
    }
    else if(node->node_elem.type == OPER)
    {
        fprintf(graph, 
                "node [shape=\"box\", style=\"filled\", fillcolor=\"%s\", fontcolor=\"%s\", margin=\"0.01\"];\n", 
                oper_color, font_color);
    }
    else if(node->node_elem.type == NUM)
    {
        fprintf(graph, 
                "node [shape=\"box\", style=\"filled\", fillcolor=\"%s\", fontcolor=\"%s\", margin=\"0.01\"];\n", 
                num_color, font_color);
    }

    fprintf(graph,
            "\"node%p\" [shape=\"record\", label=\"\\n %s\"];\n", 
            node, tree->ElemPrinter(&node->node_elem));
    
    PrintGraphNode(tree, node->left,  graph);
    PrintGraphNode(tree, node->right, graph);
}

void PrintGraphLinks (Tree* tree, TreeNode* node, FILE* graph)
{
    assert(tree != NULL);
    assert(graph != NULL);
    
    if(!node)
    {
        return;
    }

    if(node->left != NULL)
    {
        fprintf(graph, 
                "\"node%p\" -> \"node%p\"  [color=\"%s\" fontcolor=\"%s\"];\n",
                node, node->left, links_color, links_color);
        PrintGraphLinks(tree, node->left, graph);
    }
    
    if(node->right != NULL)
    {
        fprintf(graph, 
                "\"node%p\" -> \"node%p\"  [color=\"%s\" fontcolor=\"%s\"];\n",
                node, node->right, links_color, links_color);
        PrintGraphLinks(tree, node->right, graph);    
    }
}

void TreeGraphPrint(Tree* tree, const char* file_name)
{
    assert(tree      != NULL);
    assert(file_name != NULL);

    const size_t MAX_FILE_NAME_SIZE       = 50;
    const size_t MAX_CONSOLE_COMMAND_SIZE = 100;
    static size_t graph_num               = 1;

    char graph_file_name[MAX_FILE_NAME_SIZE] = "";

    sprintf(graph_file_name, "%s.gv", file_name);
    FILE* graph = fopen(graph_file_name, "w");

    fprintf(graph, "digraph TreeGraph {\n");

    PrintGraphNode(tree, tree->root, graph);
    PrintGraphLinks(tree, tree->root, graph);

    fprintf(graph, "}");

    char console_command[MAX_CONSOLE_COMMAND_SIZE] = "";
    
    fclose(graph);

    sprintf(console_command, 
            "iconv -f CP1251 -t UTF-8 %s | dot -T png -o %s%zu.png", 
            graph_file_name, file_name, graph_num);
    system(console_command);

    sprintf(console_command, "start %s%zu.png", file_name, graph_num);
    system(console_command);

    graph_num++;
}

unsigned TreeVerifier(Tree* tree)
{
    if(!tree)
    {
        return TREE_PTR_NULL;
    }

    CHECK_ERROR(tree, !tree->root, ROOT_PTR_NULL);

    return 0;
}

void TreeCtor(Tree* tree, char* (*ElemPrinter) (const TreeNode_t*))
{
    assert(tree        != NULL);
    assert(ElemPrinter != NULL);

    tree->ElemPrinter = ElemPrinter;
}

int TreeDtor(Tree* tree)
{
    assert(tree != NULL);

    if(!(tree->errors & TREE_DELETED))
    {
        DestroyNode(tree, tree->root);
        tree->errors |= TREE_DELETED;
    }
    else
        return TREE_DELETED;

    return 0;
}
