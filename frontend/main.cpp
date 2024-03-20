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

    
}