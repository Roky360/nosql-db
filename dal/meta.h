#ifndef NOSQL_DB_META_H
#define NOSQL_DB_META_H

#include "../config/config.h"

namespace dal {
    class Meta {
    public:
        // Free List page number
        pgnum freelistPage;
        pgnum root;

        Meta();

        bytearray serialize(bytearray buf) const;

        void deserialize(bytearray buf);
    };
}

#endif //NOSQL_DB_META_H
