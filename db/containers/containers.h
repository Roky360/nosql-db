#ifndef NOSQL_DB_CONTAINERS_H
#define NOSQL_DB_CONTAINERS_H

#include <string>
#include "container/container.h"
#include "../../config/config.h"
#include "../../dal/node/node.h"

using namespace std;
using namespace dal;

namespace dal {
    class Node;
    class NodeItem;
    class Dal;
}

namespace db {
    class Collection;
    class Tx;

    /**
     * Container is the base class of Collections and Documents.
     * It is an abstract class that should not be instantiated.
     */
    class Container {
    public:
        string id;
        pgnum root;
        uint64 _elemCount;

        Dal *dal;
        Tx *tx; // associated transaction

        Container();

        virtual NodeItem *serialize() const;

        virtual void deserialize(NodeItem *item);

        virtual void _write() const = 0;

        /**
         * Returns the number of elements stored under this container.
         * @return
         */
        virtual uint64 count() const = 0;

        void _incCount();

        void _decCount();

        /**
         * Get an item in a document from given key.
         */
        virtual NodeItem *_get(const string &key) const;

        /**
         * Writes a key-value pair to this document.
         * If it exists it will be overwritten.
         * @return This document
         */
        virtual Container *_put(const string &key, const string &value);

        /**
         * Deletes the resource itself.
         */
        virtual void remove() = 0;

        /**
         * Removes a key-value pair from the document.
         * @param key The key of the item to _remove.
         * @return This document
         */
        virtual Container *_remove(const string &key);

        vector<Node *> _getNodes(const vector<int> &indexes);
    };

    /* Document */

    class Document : public Container {
    public:
        Collection *parentCollection;

        Document();

        Container *toContainer();

        void _write() const override;

        uint64 count() const override;

        /**
         * Removes this document from its parent collection.
         */
        void remove() override;

        /**
         * Gets a key-value pair from a document.
         * // TODO: currently only strings are supported, more more datatypes will be added
         */
        NodeItem *get(const string &key);

        /**
         * Puts a key-value pair in this document. If it already exists it will be overridden.
         * @return This document
         */
        Document *put(const string &key, const string &value);

        /**
         * Removes a key-value pair from this document.
         * If `key` doesn't exist, it will do nothing.
         * @return This document
         */
        Document *remove(const string &key);
    };

    /* Collection */

    class Collection : public Container {
    public:
        Collection();

        Container *toContainer();

        void _write() const override;

        uint64 count() const override;

        Document *_getRootDoc();

        Document *_newDoc(const string &id);

        /**
         * Deletes this collection.
         * If this collection doesn't exist in the database, it will do nothing.
         */
        void remove() override;

        /**
         * Returns a document in this collection with the provided name.
         * If the document doesn't exist
         * @param createIfAbsent Create a document with the given name if it doesn't exist at the time of calling this
         * method. If this is false, nullptr will be returned.
         */
        Document *document(const string &name, bool createIfAbsent = true);

        /**
         * Removes a document from this collection.
         * If a document with the given name does not exist, will do nothing.
         * @return This collection
         */
        Collection *removeDoc(const string &name);
    };
}

#endif //NOSQL_DB_CONTAINERS_H
