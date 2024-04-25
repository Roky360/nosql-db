#ifndef README_MD_COLLECTION_H
#define README_MD_COLLECTION_H

#include <string>
#include "../config/config.h"

using namespace std;

class Collection {
public:
    string name;
    pgnum root;     // page number of the root B-tree

    explicit Collection(const string &name);

    bytearray serialize(bytearray buf) const;

    void deserialize(bytearray buf);


};

#endif //README_MD_COLLECTION_H
