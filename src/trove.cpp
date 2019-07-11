#include "trove.h"

#if defined(_WIN32)

#include <Windows.h>

struct Trove_FSIterator_private
{
    WIN32_FIND_DATAA m_FindData;
    HANDLE m_Handle;
};

size_t Trove_GetFSIteratorSize()
{
    return sizeof(Trove_FSIterator_private);
}

static int IsSkippableFile(HTrove_FSIterator fs_iterator)
{
    const char* p = fs_iterator->m_FindData.cFileName;
    if ((*p++) != '.')
    {
        return 0;
    }
    if ((*p) == '\0')
    {
        return 1;
    }
    if ((*p++) != '.')
    {
        return 0;
    }
    if ((*p) == '\0')
    {
        return 1;
    }
    return 0;
}

static int Skip(HTrove_FSIterator fs_iterator)
{
    while (IsSkippableFile(fs_iterator))
    {
        if (FALSE == ::FindNextFileA(fs_iterator->m_Handle, &fs_iterator->m_FindData))
        {
            return 0;
        }
    }
    return 1;
}

int Trove_StartFind(HTrove_FSIterator fs_iterator, const char* path)
{
    char scan_pattern[MAX_PATH];
    strcpy(scan_pattern, path);
    strncat(scan_pattern, "\\*.*", MAX_PATH - strlen(scan_pattern));
    fs_iterator->m_Handle = ::FindFirstFileA(scan_pattern, &fs_iterator->m_FindData);
    if (fs_iterator->m_Handle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    return Skip(fs_iterator);
}

int Trove_FindNext(HTrove_FSIterator fs_iterator)
{
    if (FALSE == ::FindNextFileA(fs_iterator->m_Handle, &fs_iterator->m_FindData))
    {
        return 0;
    }
    return Skip(fs_iterator);
}

void Trove_CloseFind(HTrove_FSIterator fs_iterator)
{
    ::FindClose(fs_iterator->m_Handle);
    fs_iterator->m_Handle = INVALID_HANDLE_VALUE;
}

const char* Trove_GetFileName(HTrove_FSIterator fs_iterator)
{
    if (fs_iterator->m_FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        return 0;
    }
    return fs_iterator->m_FindData.cFileName;
}

const char* Trove_GetDirectoryName(HTrove_FSIterator fs_iterator)
{
    if (fs_iterator->m_FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        return fs_iterator->m_FindData.cFileName;
    }
    return 0;
}

HTroveOpenReadFile Trove_OpenReadFile(const char* path)
{
    HANDLE handle = ::CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (handle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    return (HTroveOpenReadFile)handle;
}

HTroveOpenWriteFile Trove_OpenWriteFile(const char* path)
{
    HANDLE handle = ::CreateFileA(path, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (handle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    return (HTroveOpenWriteFile)handle;
}

int Trove_Read(HTroveOpenReadFile handle, uint64_t offset, uint64_t length, void* output)
{
    HANDLE h = (HANDLE)(handle);
    ::SetFilePointer(h, (LONG)offset, 0, FILE_BEGIN);
    return TRUE == ::ReadFile(h, output, (LONG)length, 0, 0);
}

int Trove_Write(HTroveOpenWriteFile handle, uint64_t offset, uint64_t length, const void* input)
{
    HANDLE h = (HANDLE)(handle);
    ::SetFilePointer(h, (LONG)offset, 0, FILE_BEGIN);
    return TRUE == ::WriteFile(h, input, (LONG)length, 0, 0);
}

uint64_t Trove_GetFileSize(HTroveOpenReadFile handle)
{
    HANDLE h = (HANDLE)(handle);
    return ::GetFileSize(h, 0);
}

void Trove_CloseReadFile(HTroveOpenReadFile handle)
{
    HANDLE h = (HANDLE)(handle);
    ::CloseHandle(h);
}

void Trove_CloseWriteFile(HTroveOpenWriteFile handle)
{
    HANDLE h = (HANDLE)(handle);
    ::CloseHandle(h);
}

const char* Trove_ConcatPath(const char* folder, const char* file)
{
    size_t path_len = strlen(folder) + 1 + strlen(file) + 1;
    char* path = (char*)malloc(path_len);
    strcpy(path, folder);
    strcat(path, "\\");
    strcat(path, file);
    return path;
}

#endif

#if defined(__APPLE__) || defined(__linux__)

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct Trove_FSIterator_private
{
    DIR* m_DirStream;
    struct dirent * m_DirEntry;
};

size_t Trove_GetFSIteratorSize()
{
    return sizeof(Trove_FSIterator_private);
}

static int IsSkippableFile(HTrove_FSIterator fs_iterator)
{
    if ((fs_iterator->m_DirEntry->d_type != DT_DIR) &&
        (fs_iterator->m_DirEntry->d_type != DT_REG))
    {
        return 0;
    }
    const char* p = fs_iterator->m_DirEntry->d_name;
    if ((*p++) != '.')
    {
        return 0;
    }
    if ((*p) == '\0')
    {
        return 1;
    }
    if ((*p++) != '.')
    {
        return 0;
    }
    if ((*p) == '\0')
    {
        return 1;
    }
    return 0;
}

static int Skip(HTrove_FSIterator fs_iterator)
{
    while (IsSkippableFile(fs_iterator))
    {
        fs_iterator->m_DirEntry = readdir(fs_iterator->m_DirStream);
        if (fs_iterator->m_DirEntry == 0)
        {
            return 0;
        }
    }
    return 1;
}

int Trove_StartFind(HTrove_FSIterator fs_iterator, const char* path)
{
    fs_iterator->m_DirStream = opendir(path);
    if (0 == fs_iterator->m_DirStream)
    {
        return 0;
    }

    fs_iterator->m_DirEntry = readdir(fs_iterator->m_DirStream);
    if (fs_iterator->m_DirEntry == 0)
    {
        closedir(fs_iterator->m_DirStream);
        return 0;
    }
    return Skip(fs_iterator);
}

int Trove_FindNext(HTrove_FSIterator fs_iterator)
{
    fs_iterator->m_DirEntry = readdir(fs_iterator->m_DirStream);
    if (fs_iterator->m_DirEntry == 0)
    {
        return 0;
    }
    return Skip(fs_iterator);
}

void Trove_CloseFind(HTrove_FSIterator fs_iterator)
{
    closedir(fs_iterator->m_DirStream);
    fs_iterator->m_DirStream = 0;
}

const char* Trove_GetFileName(HTrove_FSIterator fs_iterator)
{
    if (fs_iterator->m_DirEntry->d_type != DT_REG)
    {
        return 0;
    }
    return fs_iterator->m_DirEntry->d_name;
}

const char* Trove_GetDirectoryName(HTrove_FSIterator fs_iterator)
{
    if (fs_iterator->m_DirEntry->d_type != DT_DIR)
    {
        return 0;
    }
    return fs_iterator->m_DirEntry->d_name;
}

HTroveOpenReadFile Trove_OpenReadFile(const char* path)
{
    FILE* f = fopen(path, "rb");
    return (HTroveOpenReadFile)f;
}

HTroveOpenWriteFile Trove_OpenWriteFile(const char* path)
{
    FILE* f = fopen(path, "wb");
    return (HTroveOpenWriteFile)f;
}

int Trove_Read(HTroveOpenReadFile handle, uint64_t offset, uint64_t length, void* output)
{
    FILE* f = (FILE*)handle;
    fseek(f, (int)offset, SEEK_SET);
    size_t read = fread(output, (size_t)length, 1, f);
    return read == 1u;
}

int Trove_Write(HTroveOpenWriteFile handle, uint64_t offset, uint64_t length, const void* input)
{
    FILE* f = (FILE*)handle;
    fseek(f, (int)offset, SEEK_SET);
    size_t written = fwrite(input, (size_t)length, 1, f);
    return written == 1u;
}

uint64_t Trove_GetFileSize(HTroveOpenReadFile handle)
{
    FILE* f = (FILE*)handle;
    fseek(f, 0, SEEK_END);
    return (uint64_t)ftell(f);
}

void Trove_CloseReadFile(HTroveOpenReadFile handle)
{
    FILE* f = (FILE*)handle;
    fclose(f);
}

void Trove_CloseWriteFile(HTroveOpenWriteFile handle)
{
    FILE* f = (FILE*)handle;
    fclose(f);
}

const char* Trove_ConcatPath(const char* folder, const char* file)
{
    size_t path_len = strlen(folder) + 1 + strlen(file) + 1;
    char* path = (char*)malloc(path_len);
    strcpy(path, folder);
    strcat(path, "/");
    strcat(path, file);
    return path;
}

#endif
