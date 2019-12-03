#include "trove.h"

#if defined(_WIN32)

#include <Windows.h>

void Trove_NormalizePath(char* path)
{
    while (*path)
    {
        *path++ = *path == '\\' ? '/' : *path;
    }
}

void Trove_DenormalizePath(char* path)
{
    while (*path)
    {
        *path++ = *path == '/' ? '\\' : *path;
    }
}

int Trove_CreateDirectory(const char* path)
{
    BOOL ok = ::CreateDirectoryA(path, NULL);
    return ok;
}

int Trove_MoveFile(const char* source, const char* target)
{
    BOOL ok = ::MoveFileA(source, target);
    return ok ? 1 : 0;
}

int Trove_IsDir(const char* path)
{
    DWORD attrs = ::GetFileAttributesA(path);
    if (attrs == INVALID_FILE_ATTRIBUTES)
    {
        return 0;
    }
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
}

int Trove_IsFile(const char* path)
{
    DWORD attrs = ::GetFileAttributesA(path);
    if (attrs == INVALID_FILE_ATTRIBUTES)
    {
        return 0;
    }
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

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

uint64_t Trove_GetEntrySize(HTrove_FSIterator fs_iterator)
{
    DWORD high = fs_iterator->m_FindData.nFileSizeHigh;
    DWORD low = fs_iterator->m_FindData.nFileSizeLow;
    return (((uint64_t)high) << 32) + (uint64_t)low;
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

HTroveOpenWriteFile Trove_OpenWriteFile(const char* path, int truncate)
{
    HANDLE handle = ::CreateFileA(path, GENERIC_READ | GENERIC_WRITE, 0, 0, truncate ? CREATE_ALWAYS : OPEN_ALWAYS, 0, 0);
    if (handle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    return (HTroveOpenWriteFile)handle;
}

int Trove_SetFileSize(HTroveOpenWriteFile handle, uint64_t length)
{
    HANDLE h = (HANDLE)(handle);
    LONG low = (LONG)(length & 0xffffffff);
    LONG high = (LONG)(length >> 32);
    if (INVALID_SET_FILE_POINTER == ::SetFilePointer(h, low, &high, FILE_BEGIN))
    {
        return 0;
    }
    return TRUE == ::SetEndOfFile(h);
}

int Trove_Read(HTroveOpenReadFile handle, uint64_t offset, uint64_t length, void* output)
{
    HANDLE h = (HANDLE)(handle);
    LONG low = (LONG)(offset & 0xffffffff);
    LONG high = (LONG)(offset >> 32);
    if (INVALID_SET_FILE_POINTER == ::SetFilePointer(h, low, &high, FILE_BEGIN))
    {
        return 0;
    }
    return TRUE == ::ReadFile(h, output, (LONG)length, 0, 0);
}

int Trove_Write(HTroveOpenWriteFile handle, uint64_t offset, uint64_t length, const void* input)
{
    HANDLE h = (HANDLE)(handle);
    LONG low = (LONG)(offset & 0xffffffff);
    LONG high = (LONG)(offset >> 32);
    if (INVALID_SET_FILE_POINTER == ::SetFilePointer(h, low, &high, FILE_BEGIN))
    {
        return 0;
    }
    return TRUE == ::WriteFile(h, input, (LONG)length, 0, 0);
}

uint64_t Trove_GetFileSize(HTroveOpenReadFile handle)
{
    HANDLE h = (HANDLE)(handle);
    DWORD high = 0;
    DWORD low = ::GetFileSize(h, &high);
    return (((uint64_t)high) << 32) + (uint64_t)low;
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
    size_t folder_length = strlen(folder);
    if (folder_length > 0 && folder[folder_length - 1] == '\\')
    {
        --folder_length;
    }
    size_t path_len = folder_length + 1 + strlen(file) + 1;
    char* path = (char*)malloc(path_len);

    memmove(path, folder, folder_length);
    path[folder_length] = '\\';
    strcpy(&path[folder_length + 1], file);
    return path;
}

#endif

#if defined(__APPLE__) || defined(__linux__)

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

void Trove_NormalizePath(char* )
{
    // Nothing to do
}

void Trove_DenormalizePath(char* )
{
    // Nothing to do
}

int Trove_CreateDirectory(const char* path)
{
    int err = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (err == 0)
    {
        return 1;
    }
    int e = errno;
    if (e == EEXIST)
    {
        return 1;
    }
    printf("Can't create directory `%s`: %d\n", path, e);
    return 0;
}

int Trove_MoveFile(const char* source, const char* target)
{
    int err = rename(source, target);
    if (err == 0)
    {
        return 1;
    }
    int e = errno;
    printf("Can't move `%s` to `%s`: %d\n", source, target, e);
    return 0;
}

int Trove_IsDir(const char* path)
{
    struct stat path_stat;
    int err = stat(path, &path_stat);
    if (0 == err)
    {
        return S_ISDIR(path_stat.st_mode);
    }
    int e = errno;
    if (ENOENT == e)
    {
        return 0;
    }
    printf("Can't determine type of `%s`: %d\n", path, e);
    return 0;
}

int Trove_IsFile(const char* path)
{
    struct stat path_stat;
    int err = stat(path, &path_stat);
    if (0 == err)
    {
        return S_ISREG(path_stat.st_mode);
    }
    int e = errno;
    if (ENOENT == e)
    {
        return 0;
    }
    printf("Can't determine type of `%s`: %d\n", path, e);
    return 0;
}

struct Trove_FSIterator_private
{
    char* m_DirPath;
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
    fs_iterator->m_DirPath = strdup(path);
    fs_iterator->m_DirStream = opendir(path);
    if (0 == fs_iterator->m_DirStream)
    {
        free(fs_iterator->m_DirPath);
        return 0;
    }

    fs_iterator->m_DirEntry = readdir(fs_iterator->m_DirStream);
    if (fs_iterator->m_DirEntry == 0)
    {
        closedir(fs_iterator->m_DirStream);
        free(fs_iterator->m_DirPath);
        return 0;
    }
    int has_files = Skip(fs_iterator);
    if (has_files)
    {
        return 1;
    }
    closedir(fs_iterator->m_DirStream);
    free(fs_iterator->m_DirPath);
    return 0;
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
    free(fs_iterator->m_DirPath);
    fs_iterator->m_DirPath = 0;
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

uint64_t Trove_GetEntrySize(HTrove_FSIterator fs_iterator)
{
    if (fs_iterator->m_DirEntry->d_type != DT_REG)
    {
        return 0;
    }
    size_t dir_len = strlen(fs_iterator->m_DirPath);
    size_t file_len = strlen(fs_iterator->m_DirEntry->d_name);
    char* path = (char*)malloc(dir_len + 1 + file_len + 1);
    memcpy(&path[0], fs_iterator->m_DirPath, dir_len);
    path[dir_len] = '/';
    memcpy(&path[dir_len + 1], fs_iterator->m_DirEntry->d_name, file_len);
    path[dir_len + 1 + file_len] = '\0';
    struct stat stat_buf;
    int ok = stat(path, &stat_buf);
    uint64_t size = ok ? 0 : (uint64_t)stat_buf.st_size;
    free(path);
    return size;
}

HTroveOpenReadFile Trove_OpenReadFile(const char* path)
{
    FILE* f = fopen(path, "rb");
    return (HTroveOpenReadFile)f;
}

HTroveOpenWriteFile Trove_OpenWriteFile(const char* path, int truncate)
{
    FILE* f = fopen(path, truncate ? "wb" : "ab");
    return (HTroveOpenWriteFile)f;
}

int Trove_SetFileSize(HTroveOpenWriteFile handle, uint64_t length)
{
    FILE* f = (FILE*)handle;
    fflush(f);
    int err = ftruncate(fileno(f), (off_t)length);
    if (err == 0)
    {
        fflush(f);
        uint64_t verify_size = Trove_GetFileSize((HTroveOpenReadFile)handle);
        if (verify_size != length)
        {
            printf("Truncate did not set the correct size of `%ld`\n", (off_t)length);
            return 0;
        }
        return 1;
    }
    int e = errno;
    printf("Can't truncate to `%ld`: %d\n", (off_t)length, e);
    return 0;
}

int Trove_Read(HTroveOpenReadFile handle, uint64_t offset, uint64_t length, void* output)
{
    FILE* f = (FILE*)handle;
    if (-1 == fseek(f, (long int)offset, SEEK_SET))
    {
        return 0;
    }
    size_t read = fread(output, (size_t)length, 1, f);
    return read == 1u;
}

int Trove_Write(HTroveOpenWriteFile handle, uint64_t offset, uint64_t length, const void* input)
{
    FILE* f = (FILE*)handle;
    if (-1 == fseek(f, (long int )offset, SEEK_SET))
    {
        return 0;
    }
    size_t written = fwrite(input, (size_t)length, 1, f);
    return written == 1u;
}

uint64_t Trove_GetFileSize(HTroveOpenReadFile handle)
{
    FILE* f = (FILE*)handle;
    if (-1 == fseek(f, 0, SEEK_END))
    {
        return 0;
    }
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
    fflush(f);
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
