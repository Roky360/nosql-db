#include "meta.h"
#include "../utils/ioutils.h"

using namespace ioutils;

namespace dal {
    Meta::Meta() = default;

    bytearray Meta::serialize(bytearray buf) const {
        size_t pos = 0;
        writeIntToStream(buf, pos, this->freelistPage);
        writeIntToStream(buf, pos, this->root);

        return buf;
    }

    void Meta::deserialize(bytearray buf) {
        size_t pos = 0;
        this->freelistPage = readIntFromStream(buf, pos);
        this->root = readIntFromStream(buf, pos);
    }
}
