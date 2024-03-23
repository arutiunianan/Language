#include "backend.h"

#define NODE_OPER current_node->node_elem.elem.oper
#define NODE_TYPE current_node->node_elem.type 
#define NODE_LEFT current_node->left->node_elem.elem

#define RETRANSLATE_LEFT_NODE()  RetranslateNode(tree, current_node->left,  current_locals_num, nametables, asm_code_file)
#define RETRANSLATE_RIGHT_NODE() RetranslateNode(tree, current_node->right, current_locals_num, nametables, asm_code_file)

#define RETRANSLATE_BINARY_OP(operation)                                   \
    RETRANSLATE_RIGHT_NODE();                                               \
    RETRANSLATE_LEFT_NODE();                                                 \
    fprintf(asm_code_file, "\t" #operation "\n");                             \

void RetranslateNode(Tree* tree, TreeNode* current_node, int* current_locals_num, ProgramNameTables* nametables, FILE* asm_code_file);

void RetranslateWhileNode(Tree* tree, TreeNode* current_node, int* current_locals_num, ProgramNameTables* nametables, FILE* asm_code_file)
{
    assert(tree          != NULL);
    assert(current_node  != NULL);
    assert(nametables    != NULL);
    assert(asm_code_file != NULL);

    static int while_count  = 1;
    int current_while_count = while_count;

    fprintf(asm_code_file, "\n\tstartwhile%zu:\n", current_while_count);

    RETRANSLATE_LEFT_NODE();

    fprintf(asm_code_file, "\tPUSH 0\n");
    fprintf(asm_code_file, "\tJE endwhile%zu\n", current_while_count);
    
    RETRANSLATE_RIGHT_NODE();

    fprintf(asm_code_file, "\tJMP startwhile%zu\n", current_while_count);
    fprintf(asm_code_file, "\tendwhile%zu:\n", current_while_count);

    while_count++;
}

void RetranslateIfNode(Tree* tree, TreeNode* current_node, int* current_locals_num, ProgramNameTables* nametables, FILE* asm_code_file)
{
    assert(tree          != NULL);
    assert(current_node  != NULL);
    assert(nametables    != NULL);
    assert(asm_code_file != NULL);

    static int if_count  = 1;
    int current_if_count = if_count;

    fprintf(asm_code_file, "\n\tstartif%zu:\n", current_if_count);

    RETRANSLATE_LEFT_NODE();

    fprintf(asm_code_file, "\tPUSH 0\n");
    fprintf(asm_code_file, "\tJE endif%zu\n", current_if_count);
    
    RETRANSLATE_RIGHT_NODE();

    fprintf(asm_code_file, "\tendif%zu:\n", current_if_count);

    if_count++;
}

void RetranslateFuncDefNode(Tree* tree, TreeNode* current_node, int* current_locals_num, ProgramNameTables* nametables, FILE* asm_code_file)
{
    assert(tree          != NULL);
    assert(current_node  != NULL);
    assert(nametables    != NULL);
    assert(asm_code_file != NULL);

    NameTableFind(&nametables->functions_nametable, NODE_LEFT.id->name, current_locals_num);
    
    fprintf(asm_code_file, "\n%s:\n", NODE_LEFT.id->name);

    RETRANSLATE_RIGHT_NODE();
}

void RetranslateAssignNode(Tree* tree, TreeNode* current_node, int* current_locals_num, ProgramNameTables* nametables, FILE* asm_code_file)
{
    assert(tree          != NULL);
    assert(current_node  != NULL);
    assert(nametables    != NULL);
    assert(asm_code_file != NULL);

    RETRANSLATE_RIGHT_NODE();

    fprintf(asm_code_file, "\tPOP [%d] \t; %s\n", NODE_LEFT.id->num, NODE_LEFT.id->name);
}

void RetranslateUserInputNode(Tree* tree, TreeNode* current_node, ProgramNameTables* nametables, FILE* asm_code_file)
{
    assert(tree          != NULL);
    assert(current_node  != NULL);
    assert(nametables    != NULL);
    assert(asm_code_file != NULL);

    fprintf(asm_code_file, "\tIN\n");

    fprintf(asm_code_file, "\tPOP [%d] \t; %s\n", NODE_LEFT.id->num, NODE_LEFT.id->name);
}

void RetranslateUserCOutputNode(Tree* tree, TreeNode* current_node, ProgramNameTables* nametables, FILE* asm_code_file)
{
    assert(tree          != NULL);
    assert(current_node  != NULL);
    assert(nametables    != NULL);
    assert(asm_code_file != NULL);

    fprintf(asm_code_file, "\tPUSH [%d] \t; %s\n", NODE_LEFT.id->num, NODE_LEFT.id->name);

    fprintf(asm_code_file, "\tOUT\n");
}

void RetranslateUserOutputNode(Tree* tree, TreeNode* current_node, ProgramNameTables* nametables, FILE* asm_code_file)
{
    assert(tree          != NULL);
    assert(current_node  != NULL);
    assert(nametables    != NULL);
    assert(asm_code_file != NULL);

    fprintf(asm_code_file, "\tPUSH [%d] \t; %s\n", NODE_LEFT.id->num, NODE_LEFT.id->name);

    fprintf(asm_code_file, "\tOUT\n");
}

void RetranslateReturnNode(Tree* tree, TreeNode* current_node, ProgramNameTables* nametables, FILE* asm_code_file)
{
    assert(tree          != NULL);
    assert(current_node  != NULL);
    assert(nametables    != NULL);
    assert(asm_code_file != NULL);
    
    if(current_node->left->node_elem.type == ID)
    {
        fprintf(asm_code_file, 
                "\tPUSH [%d] \t; %s\n", 
                NODE_LEFT.id->num, NODE_LEFT.id->name);
    }
    else if(current_node->left->node_elem.type == NUM)
    {
        fprintf(asm_code_file, "\tPUSH %lg\n", NODE_LEFT.num);
    }

    fprintf(asm_code_file, "\tPOP RAX\n");

    fprintf(asm_code_file, "\tRET\n");
}
void RetranslateNode(Tree* tree, TreeNode* current_node, int* current_locals_num, ProgramNameTables* nametables, FILE* asm_code_file)
{
    assert(tree          != NULL);
    assert(nametables    != NULL);
    assert(asm_code_file != NULL);

    if(!current_node)
    {
        return;
    }

    if(NODE_TYPE == OPER)
    {
        static int logic_opers_count = 0;
        int tmp_logic_opers_count    = 0;
        switch(NODE_OPER)
        {
            case WHILE:
                RetranslateWhileNode(tree, current_node, current_locals_num, nametables, asm_code_file);
                return;
            case ASSIGN:
                RetranslateAssignNode(tree, current_node, current_locals_num, nametables, asm_code_file);
                return;
            case IF:
                RetranslateIfNode(tree, current_node, current_locals_num, nametables, asm_code_file);
                return;
            case FUNC:
                RetranslateFuncDefNode(tree, current_node, current_locals_num, nametables ,asm_code_file);
                return;
            case USER_INPUT:
                RetranslateUserInputNode(tree, current_node, nametables, asm_code_file);
                return;
            case USER_OUTPUT:
                RetranslateUserOutputNode(tree, current_node, nametables, asm_code_file);
                return;
            case RETURN:
                RetranslateReturnNode(tree, current_node, nametables, asm_code_file);
                return;
            case ADD:
                RETRANSLATE_BINARY_OP(ADD);
                return;
            case MUL:
                RETRANSLATE_BINARY_OP(MUL);
                return;
            case SUB:
                RETRANSLATE_BINARY_OP(SUB);
                return;
            case DIV:
                RETRANSLATE_BINARY_OP(DIV);
                return;
            case POW:
                RETRANSLATE_BINARY_OP(POW);
                return;
            case LG:
                RETRANSLATE_RIGHT_NODE();
                RETRANSLATE_LEFT_NODE();                                                           
                fprintf(asm_code_file, "\tJBE lbe%zu\n"
                                       "\tPUSH 1\n"
                                       "\tJMP l_end%zu\n"
                                       "\tlbe%zu:\n"
                                       "\tPUSH 0\n"
                                       "\tl_end%zu:\n", 
                                       logic_opers_count, logic_opers_count, logic_opers_count, logic_opers_count);    
                logic_opers_count++;
                return;
            case LL:
                RETRANSLATE_RIGHT_NODE();
                RETRANSLATE_LEFT_NODE();                                                           
                fprintf(asm_code_file, "\tJAE lae%zu\n"
                                       "\tPUSH 1\n"
                                       "\tJMP l_end%zu\n"
                                       "\tlae%zu:\n"
                                       "\tPUSH 0\n"
                                       "\tl_end%zu:\n", 
                                       logic_opers_count, logic_opers_count, logic_opers_count, logic_opers_count);    
                logic_opers_count++;
                return;
            case LE:
                RETRANSLATE_RIGHT_NODE();
                RETRANSLATE_LEFT_NODE();                                                           
                fprintf(asm_code_file, "\tJNE lne%zu\n"
                                       "\tPUSH 1\n"
                                       "\tJMP l_end%zu\n"
                                       "\tlne%zu:\n"
                                       "\tPUSH 0\n"
                                       "\tl_end%zu:\n", 
                                       logic_opers_count, logic_opers_count, logic_opers_count, logic_opers_count);    
                logic_opers_count++;
                return;
            case LNE:
                RETRANSLATE_RIGHT_NODE();    
                RETRANSLATE_LEFT_NODE();                                               
                fprintf(asm_code_file, "\tJE le%zu\n"
                                       "\tPUSH 1\n"
                                       "\tJMP l_end%zu\n"
                                       "\tle%zu:\n"
                                       "\tPUSH 0\n"
                                       "\tl_end%zu:\n", 
                                       logic_opers_count, logic_opers_count, logic_opers_count, logic_opers_count);    
                logic_opers_count++;
                return;

            case AND:
                RETRANSLATE_RIGHT_NODE();    
                fprintf(asm_code_file, "\tPUSH 0\n"
                                       "\tJE false%zu\n", logic_opers_count);

                tmp_logic_opers_count = logic_opers_count;

                RETRANSLATE_LEFT_NODE();
                fprintf(asm_code_file, "\tPUSH 0\n"
                                       "\tJE false%zu\n"
                                       "\tPUSH 1\n"
                                       "\tJMP and_end%zu\n"
                                       "\tfalse%zu:\n"
                                       "\tPUSH 0\n"
                                       "\tand_end%zu:\n", 
                                       tmp_logic_opers_count, tmp_logic_opers_count, tmp_logic_opers_count, tmp_logic_opers_count);
                logic_opers_count++;
                return;
            case OR:
                RETRANSLATE_RIGHT_NODE();    
                fprintf(asm_code_file, "\tPUSH 0\n"
                                       "\tJNE true%zu\n", logic_opers_count);

                tmp_logic_opers_count = logic_opers_count;

                RETRANSLATE_LEFT_NODE();
                fprintf(asm_code_file, "\tPUSH 0\n"
                                       "\tJNE true%zu\n"
                                       "\tPUSH 0\n"
                                       "\tJMP or_end%zu\n"
                                       "\ttrue%zu:\n"
                                       "\tPUSH 1\n"
                                       "\tor_end%zu:\n", 
                                       tmp_logic_opers_count, tmp_logic_opers_count, tmp_logic_opers_count, tmp_logic_opers_count);
                logic_opers_count++;
                return;
            case TAB:
                break;
            case FUNC_SEP:
                break;
            default:
                break;
        }
    }

    if(current_node->node_elem.type == ID && current_node->node_elem.elem.id->type == function)
    {
        NameTable* current_local_nametable = &nametables->local_nametables[*current_locals_num];
        for(int i = 0; i < current_local_nametable->size; i++)
        {
            fprintf(asm_code_file, 
                    "\tPUSH [%d] \t; %s\n", 
                    current_local_nametable->elems[i].num, current_local_nametable->elems[i].name);
        }
    }

    RETRANSLATE_LEFT_NODE();
    RETRANSLATE_RIGHT_NODE();

    switch(current_node->node_elem.type)
    {
        case NUM:
            fprintf(asm_code_file, "\tPUSH %lg\n", current_node->node_elem.elem.num);
            break;
        case ID:
            if(current_node->node_elem.elem.id->type == function)
            {
                int         func_locals_number    = 0;
                char*          func_str             = current_node->node_elem.elem.id->name;
                NameTableElem* func                 = NameTableFind(&nametables->functions_nametable, func_str, &func_locals_number);
                int         func_params_count    = func->num;
                NameTable*     func_local_nametable = &nametables->local_nametables[func_locals_number];

                if(func_params_count)
                    for(int i = func_params_count; i > 0; i--)
                    {
                        fprintf(asm_code_file, 
                                "\tPOP [%d] \t; %s\n", 
                                func_local_nametable->elems[i-1].num, func_local_nametable->elems[i-1].name);
                    }

                fprintf(asm_code_file, "\tCALL %s\n", current_node->node_elem.elem.id->name);
                
                NameTable* current_local_nametable = &nametables->local_nametables[*current_locals_num];
                for(int i = current_local_nametable->size; i > 0; i--)
                {
                    fprintf(asm_code_file, 
                            "\tPOP [%d] \t; %s\n", 
                            current_local_nametable->elems[i-1].num, current_local_nametable->elems[i-1].name);
                }

                fprintf(asm_code_file, "\tPUSH RAX\n");
            }
            else if(current_node->node_elem.elem.id->type == variable)
            {
                fprintf(asm_code_file, 
                        "\tPUSH [%d] \t; %s\n", 
                        current_node->node_elem.elem.id->num, current_node->node_elem.elem.id->name);
            }
            else
            {
                assert(0);
            }
            break;
        case OPER:
            break;
        default:
            assert(0);
    }
}

void RetranslateTree(Tree* tree, ProgramNameTables* nametables, FILE* asm_code_file)
{
    assert(tree          != NULL);
    assert(nametables    != NULL);
    assert(asm_code_file != NULL);

    fprintf(asm_code_file, 
            "CALL %s\n"
            "HLT\n", MAIN_FUNC_NAME);
    
    int current_locals_num = 0;

    RetranslateNode(tree, tree->root, &current_locals_num, nametables, asm_code_file);
}

void ReadTreeAndNameTables(ProgramNameTables* nametables, Tree* tree, const char* ast_file_name)
{
    assert(nametables    != NULL);
    assert(tree          != NULL);
    assert(ast_file_name != NULL);

    FILE* ast_file = fopen(ast_file_name, "r");
    assert(ast_file      != NULL);

    int errors = 0;

    setvbuf(ast_file, NULL, _IOFBF, GetFileSize(ast_file, 0));

    ReadProgramNameTables(nametables, ast_file, &errors);
    ReadSYNTree(tree, nametables, ast_file, &errors);
}

#undef RETRANSLATE_BINARY_OP