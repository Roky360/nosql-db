#include "collection.h"
#include "../utils/ioutils.h"

using namespace ioutils;

Collection::Collection(const string &name) : name(name) {}

bytearray Collection::serialize(bytearray buf) const {
    size_t pos = 0;
    writeIntToStream(buf, pos, (__int16) this->name.length());
    this->name.copy(buf, this->name.length());
    writeIntToStream(buf, pos, this->root);
    return buf;
}

void Collection::deserialize(bytearray buf) {
    size_t pos = 0;
    __int16 nameLen = readIntFromStream<__int16>(buf, pos);
    this->name = string(buf + pos, nameLen);
    pos += nameLen;
    this->root = readIntFromStream(buf, pos);
}



