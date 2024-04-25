#ifndef NOSQL_DB_IO_UTILS_H
#define NOSQL_DB_IO_UTILS_H

#include <vector>
#include "../config/config.h"

using namespace std;

namespace ioutils {
    /**
     * Inserts `num` to `arr` in little endian, starting from `pos`.
     * Assumes there is enough memory for this operation.
     * @return The given array.
     */
    template<typename T>
    bytearray writeIntToStream(bytearray arr, size_t &pos, T num, bool updatePos = true) {
        for (int i = 0; i < sizeof(T); i++) {
            arr[pos + i] = (char) ((num >> (i * 8)) & 0xFF);
        }
        if (updatePos) pos += sizeof(T);
        return arr;
    }

    /**
     * Reads a uint64 number from `arr` in little endian, starting from `fromPos` and returns it.
     */
    template<typename T=uint64>
    T readIntFromStream(bytearray arr, size_t &fromPos, bool updatePos = true) {
        T num = 0;
        for (int i = 0; i < sizeof(T); i++) {
            num |= (unsigned char) (arr[fromPos + i]) << (i * 8);
        }
        if (updatePos) fromPos += sizeof(T);
        return num;
    }
}

#endif //NOSQL_DB_IO_UTILS_H
