#ifndef ERRORS_H
#define ERRORS_H

struct Error {
    int error;
    const char *description;
};

enum TypeError {
    ERROR_NO =                          0,
    ERROR_ALLOC_FAIL =                  1 << 0,
    ERROR_MEMCPY_FAIL =                 1 << 1,
    ERROR_EMPTY_POINTER =               1 << 2,
    ERROR_INVALID_FILE =                1 << 3,
    ERROR_INVALID_TREE =                1 << 4,
    ERROR_EMPTY_NODE =                  1 << 5,
    ERROR_READ_INPUT =                  1 << 6,
    ERROR_READ_FILE =                   1 << 7,
    ERROR_OPEN_FILE =                   1 << 8,
    ERROR_INVALID_INPUT =               1 << 9,
};

const Error ERRORS[] = {
    {ERROR_NO,                          "no errors"},
    {ERROR_ALLOC_FAIL,                  "realloc fail"},
    {ERROR_MEMCPY_FAIL,                 "memcpy fail"},
    {ERROR_EMPTY_POINTER,               "free NULL"},
    {ERROR_INVALID_FILE,                "invalid name of file"},
    {ERROR_INVALID_TREE,                "invalid pointer to tree"},
    {ERROR_EMPTY_NODE,                  "invalid node in tree"},
    {ERROR_READ_INPUT,                  "invalid input argument"},
    {ERROR_READ_FILE,                   "invalid data file"},
    {ERROR_OPEN_FILE,                   "invalid name of file"},
    {ERROR_INVALID_INPUT,               "invalid input"},
};
const int COUNT_ERRORS = sizeof(ERRORS) / sizeof(Error);

#define err_dump(err) err_dump_(err, __FILE__, __PRETTY_FUNCTION__, __LINE__) 
void err_dump_(int err, const char *file, const char *func, int line);

#endif // ERRORS_H