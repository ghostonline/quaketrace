#pragma once

#include <cstdio>
#include <utility>
#include <vector>

class File
{
    File() : fptr(nullptr) {}
    File(FILE* fptr) : fptr(fptr) {}

    FILE* fptr;

public:
    enum Origin
    {
        ORIGIN_START = SEEK_SET,
        ORIGIN_CURRENT = SEEK_CUR,
        ORIGIN_END = SEEK_END,
    };
    ~File() { if (fptr) { fclose(fptr); } fptr = nullptr; }
    File(File&& other) { std::swap(this->fptr, other.fptr); }
    File(const File& other) = delete;
    File& operator=(const File& other) = delete;
    File& operator=(File&& other) { std::swap(this->fptr, other.fptr); return *this; }

    static File open(const char* filename);
    static File openW(const char* filename);

    bool isValid() const { return fptr != nullptr; }
    size_t write(const void* data, size_t bytecount);
    size_t read(void* buffer, size_t bytecount);
    int seek(size_t offset, Origin origin = ORIGIN_CURRENT);
    size_t size();
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

inline size_t File::write(const void* data, size_t bytecount)
{
    size_t written = fwrite(data, bytecount, 1, fptr);
    fflush(fptr);
    return written;
}

inline size_t File::read(void* buffer, size_t bytecount)
{
    return fread(buffer, bytecount, 1, fptr);
}

inline int File::seek(size_t offset, Origin origin)
{
    return fseek(fptr, offset, origin);
}

inline size_t File::size()
{
    long pos = ftell(fptr);
    fseek(fptr, 0, SEEK_END);
    long size = ftell(fptr);
    fseek(fptr, pos, SEEK_SET);
    return static_cast<size_t>(size);
}
