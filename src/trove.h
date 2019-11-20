#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct Trove_FSIterator_private* HTrove_FSIterator;

size_t Trove_GetFSIteratorSize();

void Trove_NormalizePath(char* path);
void Trove_DenormalizePath(char* path);
int Trove_CreateDirectory(const char* path);
int Trove_MoveFile(const char* source, const char* target);
int Trove_IsDir(const char* path);
int Trove_IsFile(const char* path);

int Trove_StartFind(HTrove_FSIterator fs_iterator, const char* path);
int Trove_FindNext(HTrove_FSIterator fs_iterator);
void Trove_CloseFind(HTrove_FSIterator fs_iterator);
const char* Trove_GetFileName(HTrove_FSIterator fs_iterator);
const char* Trove_GetDirectoryName(HTrove_FSIterator fs_iterator);

typedef struct Trove_OpenReadFile_private* HTroveOpenReadFile;
typedef struct Trove_OpenWriteFile_private* HTroveOpenWriteFile;

HTroveOpenReadFile Trove_OpenReadFile(const char* path);
HTroveOpenWriteFile Trove_OpenWriteFile(const char* path, int truncate);
int Trove_SetFileSize(HTroveOpenWriteFile handle, uint64_t length);
int Trove_Read(HTroveOpenReadFile handle, uint64_t offset, uint64_t length, void* output);
int Trove_Write(HTroveOpenWriteFile handle, uint64_t offset, uint64_t length, const void* input);
uint64_t Trove_GetFileSize(HTroveOpenReadFile handle);
void Trove_CloseReadFile(HTroveOpenReadFile handle);
void Trove_CloseWriteFile(HTroveOpenWriteFile handle);
// Not sure about doing memory allocation here...
const char* Trove_ConcatPath(const char* folder, const char* file);
