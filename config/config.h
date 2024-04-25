#ifndef NOSQL_DB_CONFIG_H
#define NOSQL_DB_CONFIG_H

#include <vector>

using namespace std;

typedef unsigned long long uint64;
typedef unsigned long long pgnum;
typedef char* bytearray;
typedef vector<char> bytelist;

#define DEFAULT_PAGE_SIZE 4096
/// the size of a page number (in bytes) * related to pgnum ^
#define PAGE_NUM_SIZE 8
#define INITIAL_PAGE 0
#define META_PAGE_NUM 0

// node header size in bytes
#define NODE_HEADER_SIZE 3

#define DB_DEFAULT_FILE_EXTENSION "db"
#define MAGIC_NUMBER 0x00000000

#endif //NOSQL_DB_CONFIG_H
