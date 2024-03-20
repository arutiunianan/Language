#include "nametable.h"

const int default_expr = 0;

NameTableElem* NameTableAdd(NameTable* nametable, char* name, NameTableElemType type, int num)
{
    assert(nametable != NULL);
    assert(name      != NULL);

    strcpy(nametable->elems[nametable->size].name, name);
    nametable->elems[nametable->size].expr = default_expr;
    nametable->elems[nametable->size].type = type;
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
	for(size_t i = 0; i < nametable->size; i++)
	{
		if(strcmp(nametable->elems[i].name, name) == 0)
        {
			return &nametable->elems[i];
        }
	}

	return NULL;
}

NameTableElem* NameTableAddWithExpr(NameTable* nametable, char* name, NameTableElemType type, int new_expr, int num)
{
    NameTableElem* elem = NameTableAdd(nametable, name, type, num);
    if(elem)
    {
        elem->expr = new_expr;
    }

    return elem;
}

NameTableElem* NameTableChangeExpr(NameTable* nametable, char* name, int new_expr)
{
    NameTableElem* elem = NameTableFind(nametable, name);
    if(elem && elem->type == variable)
    {
        elem->expr = new_expr;;
    }

    return elem;
}

void WriteNameTable(NameTable* nametable, const char* nametable_header, FILE* output_file)
{
	assert(nametable != nullptr);

	fprintf(output_file, "%s [%zu] {\n", nametable_header, nametable->size);

	for (size_t i = 0; i < nametable->size; i++) 
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

void ProgramNameTablesCtor(ProgramNameTables* nametables, size_t local_nametables_counter)
{
	assert(nametables != NULL);

	nametables->local_nametables = (NameTable*)calloc(local_nametables_counter, sizeof(NameTable));
	assert(nametables->local_nametables != NULL);

	nametables->local_nametables_counter = local_nametables_counter;
	nametables->functions_nametable.size = 0;
	
	for(size_t i = 0; i < local_nametables_counter; i++)
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
		var_id = NameTableAdd(&CURRENT_LOCAL_NAMETABLE, var_name, variable, CURRENT_LOCAL_NAMETABLE.size); 
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
		func_id = NameTableAdd(&nametables->functions_nametable, func_name, function, FUNCS_COUNT);
	}
	
	return func_id;
}

void WriteProgramNameTables(ProgramNameTables* nametables, FILE* input_file)
{
	assert(nametables != NULL);
	assert(input_file != NULL);

	fprintf(input_file, "Locals Count:%zu\n\n", CURRENT_LOCAL_NUM);

	WriteNameTable(&nametables->functions_nametable, "FunctionsNameTable", input_file);

	for(size_t i = 0; i < CURRENT_LOCAL_NUM; i++)
    {
		WriteNameTable(&nametables->local_nametables[i], "LocalNameTable", input_file);
    }
}
