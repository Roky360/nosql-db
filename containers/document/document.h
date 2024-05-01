#ifndef NOSQL_DB_DOCUMENT_H
#define NOSQL_DB_DOCUMENT_H

#include <string>
#include "../../config/config.h"
#include "../../dal/node/node.h"

using namespace std;
using namespace dal;

class Document {
public:
    string id;
    pgnum root;

    Dal *dal;

    /* Methods */
    Document();

    NodeItem *serialize() const;

    void deserialize(NodeItem *item);

    /**
     * Get an item in a document from given key.
     */
    NodeItem *get(const string &key) const;

    /**
     * Writes a key-value pair to this document.
     * If it exists it will be overwritten.
     * @return This document
     */
    Document *put(const string &key, const string &value);

    /**
     * Removes a key-value pair from the document.
     * @param key The key of the item to remove.
     * @return This document
     */
    Document *remove(const string &key);
};

#endif //NOSQL_DB_DOCUMENT_H
