#include "../lang_lib/str_lib/io.h"
#include "../lang_lib/nametable.h"

int SkipTabs(char* code, int if_number)
{
    assert(code != NULL);
    assert(if_number >= 0);
    for(int i = 0; i < if_number * 4; i++)
    {
        if(code[i] != ' ')
        {
            return 1;
        }
    }
    return 0;
}

int main(int argc, const char* argv[])
{
    /*int if_number = 1;
    FILE* file = fopen("xz.txt", "rb");

    int asm_code_size = GetFileSize(file, SEEK_SET);
    char* asm_code = (char*)calloc(asm_code_size + 1, sizeof(char));

    fread(asm_code, sizeof(char), asm_code_size, file);
    asm_code[asm_code_size] = '\0';
    
    if(SkipTabs(asm_code + 5, if_number))
    {
        printf("ошибка((\n");
    }
    else
    {
        printf("%c\n", *(asm_code + 5 + if_number * 4));
    }*/

    NameTable nametable = {};
    NameTableCtor(&nametable);

    NameTableAddWithExpr(&nametable, "hihi", variable, 1);
    NameTableAddWithExpr(&nametable, "haha", variable, 2);
    for(int i = 0; i < nametable.size; i++)
    {
        printf("int %s = %d\n", nametable.elems[i].name, nametable.elems[i].expr);
    }
}