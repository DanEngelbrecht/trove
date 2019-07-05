#include "../src/trove.h"

#include <memory>
#include <stdio.h>
#include <assert.h>

#include "../third-party/jctest/src/jc_test.h"

#if defined(_WIN32)
    #include <malloc.h>
    #define alloca _alloca
#else
    #include <alloca.h>
#endif

TEST(Nadir, IterateFolder)
{
    HTrove_FSIterator iterator = (HTrove_FSIterator)alloca(Trove_GetFSIteratorSize());
    if (Trove_StartFind(iterator, "."))
    {
        do
        {
            if (const char* filename = Trove_GetFileName(iterator))
            {
                printf("\nFound file `%s`", filename);
            }
            else if (const char* dirname = Trove_GetFileName(iterator))
            {
                printf("\nFound folder `%s`", dirname);
            }
        }while(Trove_FindNext(iterator));
        Trove_CloseFind(iterator);
    }
}
