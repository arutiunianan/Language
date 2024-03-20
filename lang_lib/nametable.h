#ifndef NT_H
#define NT_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define FUNCS_COUNT              nametables->functions_nametable.size
#define FUNCS_NAMETABLE          nametables->functions_nametable
#define CURRENT_LOCAL_NUM        nametables->local_nametables_counter
#define CURRENT_LOCAL_NAMETABLE  nametables->local_nametables[FUNCS_COUNT - 1]

const int MAX_NAME_SIZE      = 100;
const int MAX_NAMETABLE_SIZE = 200;

enum NameTableElemType
{
    variable   = 1, 
    function   = 2, 
    error_type = 0
};

struct NameTableElem
{
    NameTableElemType type;
    char name[MAX_NAME_SIZE];
    int expr;
    int num;
};

struct NameTable
{
    NameTableElem elems[MAX_NAMETABLE_SIZE];
    int size;
};

struct ProgramNameTables
{
    NameTable  functions_nametable;
    NameTable* local_nametables;
    //NameTable  common_nametable;

    size_t local_nametables_counter;
};

void NameTableCtor(NameTable* nametable);
void NameTableDtor(NameTable* nametable);

NameTableElem* NameTableAdd(NameTable* nametable, char* name, NameTableElemType type, int num);
NameTableElem* NameTableFind(NameTable* nametable, char* name);
NameTableElem* NameTableAddWithExpr(NameTable* nametable, char* name, NameTableElemType type, int new_expr, int num);
NameTableElem* NameTableChangeExpr(NameTable* nametable, char* name, int new_expr);
void WriteNameTable(NameTable* nametable, const char* nametable_header, FILE* output_file);

void ProgramNameTablesCtor(ProgramNameTables* nametables, size_t local_nametables_counter);
void ProgramNameTablesDtor(ProgramNameTables* nametables);

NameTableElem* ProgramNameTablesAddVar(ProgramNameTables* nametables, char* var_name);
NameTableElem* ProgramNameTablesAddFunc(ProgramNameTables* nametables, char* func_name);
void WriteProgramNameTables(ProgramNameTables* nametables, FILE* input_file);

#endif // NT_H