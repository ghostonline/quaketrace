#pragma once

#include <cstdio>
#include <utility>

class File
{
    File() : fptr(nullptr) {}
    File(FILE* fptr) : fptr(fptr) {}

    FILE* fptr;

public:
    ~File() { fclose(fptr); fptr = nullptr; }
    File(File&& other) { std::swap(this->fptr, other.fptr); }
    File(const File& other) = delete;
    File& operator=(const File& other) = delete;
    File& operator=(File&& other) { std::swap(this->fptr, other.fptr); return *this; }

    static File open(const char* filename);
    static File openW(const char* filename);

    int write(const void* data, size_t bytecount);
};

inline File File::open(const char* filename)
{
    FILE* fptr = fopen(filename, "rb");
    return File(fptr);
}

inline File File::openW(const char* filename)
{
    FILE* fptr = fopen(filename, "wb");
    return File(fptr);
}

int File::write(const void* data, size_t bytecount)
{
    return fwrite(data, bytecount, 1, fptr);
}

