#include "../src/trove.h"

#include <memory.h>
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
    bool empty_folder_found = false;
    bool something_folder_found = false;
    bool something_10chars_found = false;
    bool something_24hars_found = false;
    if (Trove_StartFind(iterator, "./test_data"))
    {
        do
        {
            if (const char* filename = Trove_GetFileName(iterator))
            {
                if (0 == strcmp(filename, "24chars.txt"))
                {
                    ASSERT_FALSE(something_24hars_found);
                    something_24hars_found = true;
                    ASSERT_EQ(24, Trove_GetEntrySize(iterator));
                }
                else
                {
                    printf("Found unexpected file: `%s`", filename);
                    ASSERT_TRUE(false);
                }
            }
            else if (const char* dirname = Trove_GetDirectoryName(iterator))
            {
                if (0 == strcmp(dirname, "empty_folder"))
                {
                    ASSERT_FALSE(empty_folder_found);
                    empty_folder_found = true;
                    ASSERT_EQ(0, Trove_GetEntrySize(iterator));
                }
                else if (0 == strcmp(dirname, "something_folder"))
                {
                    ASSERT_FALSE(something_folder_found);
                    something_folder_found = true;
                    ASSERT_EQ(0, Trove_GetEntrySize(iterator));
                }
                else
                {
                    printf("Found unexpected directory: `%s`", dirname);
                    ASSERT_TRUE(false);
                }
            }
        }while(Trove_FindNext(iterator));
        Trove_CloseFind(iterator);
    }
    ASSERT_TRUE(empty_folder_found);
    ASSERT_TRUE(something_folder_found);
    ASSERT_FALSE(something_10chars_found);
    ASSERT_TRUE(something_24hars_found);
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
        HTroveOpenWriteFile f = Trove_OpenWriteFile(test_file_path, 0);
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

TEST(Nadir, WriteFileInitialSize)
{
    const char* test_file_path = Trove_ConcatPath("build", ".test");
    const char* data = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ,.-_";
    {
        HTroveOpenWriteFile f = Trove_OpenWriteFile(test_file_path, 40);
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
