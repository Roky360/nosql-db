#ifndef NOSQL_DB_FREELIST_H
#define NOSQL_DB_FREELIST_H

#include <vector>
#include "../config/config.h"

using namespace std;

namespace dal {
    class FreeList {
    public:
        pgnum maxPage;
        vector<pgnum> releasedPages;

        FreeList();

        pgnum getNextPageNum();

        void releasePage(pgnum pageNum);

        bytearray serialize(bytearray buf);

        void deserialize(bytearray buf);
    };
}

#endif //NOSQL_DB_FREELIST_H
