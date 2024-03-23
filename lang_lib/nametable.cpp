#include "nametable.h"

const int default_expr = 0;

NameTableElem* NameTableAdd(NameTable* nametable, char* name, int num)
{
    assert(nametable != NULL);
    assert(name      != NULL);

    strcpy(nametable->elems[nametable->size].name, name);
    nametable->elems[nametable->size].expr = default_expr;
    nametable->elems[nametable->size].num  = num;
    nametable->size++;

    if(nametable->size == MAX_NAMETABLE_SIZE)
    {
        return NULL;
    }

    return &nametable->elems[nametable->size - 1];
}

NameTableElem* NameTableFind(NameTable* nametable, char* name)
{
    for(int i = 0; i < nametable->size; i++)
    {
        if(strcmp(nametable->elems[i].name, name) == 0)
        {
            return &nametable->elems[i];
        }
    }

    return NULL;
}

NameTableElem* NameTableFind(NameTable* nametable, const char* elem_to_find, int* elem_num)
{
	for(size_t i = 0; i < nametable->size; i++)
	{
		if(strcmp(nametable->elems[i].name, elem_to_find) == 0)
		{
			*elem_num = i;
			return &nametable->elems[i];
		}
	}

	return NULL;
}

void WriteNameTable(NameTable* nametable, const char* nametable_header, FILE* output_file)
{
    assert(nametable != NULL);

    fprintf(output_file, "%s [%zu] {\n", nametable_header, nametable->size);

    for(int i = 0; i < nametable->size; i++) 
    {
        fprintf(output_file, "    [\"%s\", %d]\n", 
                nametable->elems[i].name, nametable->elems[i].num);
    }

    fprintf(output_file, "}\n\n");
}

void NameTableCtor(NameTable* nametable)
{
    assert(nametable != NULL);
    nametable->size = 0;
}

void NameTableDtor(NameTable* nametable)
{
    assert(nametable != NULL);

    for(int i = 0; i < nametable->size; i++)
    {
        strcpy(nametable->elems[i].name, "Goddaaaaaaaam");
        nametable->elems[i].expr = 0xB0BA;
        nametable->elems[i].num  = 0xB1BA;
        nametable->elems[i].type = error_type;
    }
    nametable->size = 0xB1BA;
}

void ProgramNameTablesCtor(ProgramNameTables* nametables, int local_nametables_counter)
{
    assert(nametables != NULL);

    nametables->local_nametables = (NameTable*)calloc(local_nametables_counter, sizeof(NameTable));
    assert(nametables->local_nametables != NULL);

    nametables->local_nametables_counter = local_nametables_counter;
    nametables->functions_nametable.size = 0;
    
    for(int i = 0; i < local_nametables_counter; i++)
    {
        nametables->local_nametables[i].size = 0;
    }
}

void ProgramNameTablesDtor(ProgramNameTables* nametables)
{
    assert(nametables != NULL);

    free(nametables->local_nametables);
}

NameTableElem* ProgramNameTablesAddVar(ProgramNameTables* nametables, char* var_name)
{
    assert(nametables != NULL);
    assert(var_name   != NULL);

    NameTableElem* var_id = NameTableFind(&CURRENT_LOCAL_NAMETABLE, var_name);
    if(!var_id)
    {
        var_id = NameTableAdd(&CURRENT_LOCAL_NAMETABLE, var_name, CURRENT_LOCAL_NAMETABLE.size); 
        var_id->type = variable;
    }
    
    return var_id;
}

NameTableElem* ProgramNameTablesAddFunc(ProgramNameTables* nametables, char* func_name)
{
    assert(nametables != NULL);
    assert(func_name  != NULL);

    NameTableElem* func_id = NameTableFind(&FUNCS_NAMETABLE, func_name);
    if(!func_id)
    {
        func_id = NameTableAdd(&nametables->functions_nametable, func_name, FUNCS_COUNT);
        func_id->type = function;
    }
    
    return func_id;
}

void WriteProgramNameTables(ProgramNameTables* nametables, FILE* input_file)
{
    assert(nametables != NULL);
    assert(input_file != NULL);

    fprintf(input_file, "Locals Count:%zu\n\n", CURRENT_LOCAL_NUM);

    WriteNameTable(&nametables->functions_nametable, "FunctionsNameTable", input_file);

    for(int i = 0; i < CURRENT_LOCAL_NUM; i++)
    {
        WriteNameTable(&nametables->local_nametables[i], "LocalNameTable", input_file);
    }
}


#define CHECK_VALID_ARGS_NUM(_fscanf)      \
	{                                       \
		valid_args_num = _fscanf;            \
		if(!(valid_args_num))                 \
		{                                      \
			*errors |= 1;                       \
			return;                              \
		}                                         \
	}

void ReadNameTable(NameTable* nametable, FILE* ast_file, const char* needed_nametable_header, int* errors)
{
	assert(nametable != NULL);
	assert(ast_file  != NULL);

	size_t valid_args_num = 0;
	int cmd_len           = 0;
	
	char nametable_header[MAX_NAME_SIZE] = "";
	CHECK_VALID_ARGS_NUM(fscanf(ast_file, " %s [ %zu ] {", nametable_header, &nametable->size));

	if(strcmp(needed_nametable_header, nametable_header) != 0)
    {
		*errors |= 1;
    }

	for(size_t i = 0; i < nametable->size; i++)
	{
		CHECK_VALID_ARGS_NUM(fscanf(ast_file, " [ \" %[^\"] \" , %d]", 
		nametable->elems[i].name, &nametable->elems[i].num));
	}

	(void)fscanf(ast_file, " }%n", &cmd_len);

	if(!cmd_len)
    {
		*errors |= 1;
    }
}

#undef CHECK_VALID_ARGS_NUM

void ReadProgramNameTables(ProgramNameTables* nametables, FILE* ast_file, int* errors)
{
	assert(nametables != NULL);
	assert(ast_file   != NULL);
	assert(errors     != NULL);

	assert(fscanf(ast_file, "Locals Count: %zu", &nametables->local_nametables_counter) == 1);
	ProgramNameTablesCtor(nametables, nametables->local_nametables_counter);

	ReadNameTable(&nametables->functions_nametable, ast_file, "FunctionsNameTable", errors);

	for(size_t i = 0; i < nametables->local_nametables_counter; i++)
    {
		ReadNameTable(&nametables->local_nametables[i], ast_file, "LocalNameTable", errors);
    }
}