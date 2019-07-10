#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct Trove_FSIterator_private* HTrove_FSIterator;

size_t Trove_GetFSIteratorSize();

int Trove_StartFind(HTrove_FSIterator fs_iterator, const char* path);
int Trove_FindNext(HTrove_FSIterator fs_iterator);
void Trove_CloseFind(HTrove_FSIterator fs_iterator);
const char* Trove_GetFileName(HTrove_FSIterator fs_iterator);
const char* Trove_GetDirectoryName(HTrove_FSIterator fs_iterator);

typedef struct Trove_OpenReadFile_private* HTroveOpenReadFile;
typedef struct Trove_OpenWriteFile_private* HTroveOpenWriteFile;

// Not sure about doing memory allocation here...
const char* Trove_ConcatPath(const char* folder, const char* file);
