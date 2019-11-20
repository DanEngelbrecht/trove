#include "../src/trove.h"

#include <memory>
#include <stdio.h>
#include <assert.h>
#include <string.h>

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
            else if (const char* dirname = Trove_GetDirectoryName(iterator))
            {
                printf("\nFound folder `%s`", dirname);
            }
        }while(Trove_FindNext(iterator));
        Trove_CloseFind(iterator);
    }
}

TEST(Nadir, ReadFile)
{
    HTroveOpenReadFile f = Trove_OpenReadFile("LICENSE");
    ASSERT_NE((HTroveOpenReadFile)0, f);
    uint64_t size = Trove_GetFileSize(f);
    ASSERT_GT(2000u, size);
    ASSERT_LT(1000u, size);
    char buf[12];
    ASSERT_NE(0, Trove_Read(f, 0, 11, buf));
    buf[11] = 0;
    ASSERT_EQ(0, strcmp(buf, "MIT License"));
    Trove_CloseReadFile(f);
}

TEST(Nadir, WriteFile)
{
    const char* test_file_path = Trove_ConcatPath("build", ".test");
    const char* data = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ,.-_";
    {
        HTroveOpenWriteFile f = Trove_OpenWriteFile(test_file_path, 1);
        ASSERT_NE((HTroveOpenWriteFile)0, f);
        ASSERT_NE(0, Trove_Write(f, 0, 40, data));
        Trove_CloseWriteFile(f);
    }

    HTroveOpenReadFile f = Trove_OpenReadFile(test_file_path);
    ASSERT_NE((HTroveOpenReadFile)0, f);
    uint64_t size = Trove_GetFileSize(f);
    ASSERT_EQ(40u, size);
    char buf[41];
    ASSERT_NE(0, Trove_Read(f, 0, 40, buf));
    buf[40] = 0;
    ASSERT_EQ(0, strcmp(buf, data));

    ASSERT_NE(0, Trove_Read(f, 10, 3, buf));
    buf[3] = 0;
    ASSERT_EQ(0, strcmp(buf, "ABC"));

    Trove_CloseReadFile(f);

    free((char*)test_file_path);
}
