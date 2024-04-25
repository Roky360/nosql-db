#ifndef NOSQL_DB_DB_H
#define NOSQL_DB_DB_H

#include "../config/options.h"
#include "../dal/Dal.h"
#include "../collection/collection.h"
#include "../document/document.h"

using namespace dal;

namespace db {
    class DB {
        Dal *dal;

        DB() = default;

        ~DB();

        void open(const string &path, Options& options=Options::defaultOptions);

        void close();

        // planning of needed functions
        string createCollection(const string &name="");
        Collection *getCollection(const string &name); // maybe replace with CollectionSnapshot object
        void deleteCollection(const string &name);

        // on Collection snapshot class
        Document getDocument(const string &id); // replace with DocumentSnapshot object
        vector<Document> where(); // get all documents that satisfy a certain condition
        Collection* deleteDocument(const string &id);
        void deleteCollection(); // deletes the collection itself

        // on DocumentSnapshot
        Document* get(); // returns the entire doc as DocumentSnapshot
        string get(const string &key); // returns a value from given key. * more data types will be added
        Document* put(const string &key, const string &value); // puts key-value in `doc`; overrides data if `key` exists.
        void deleteDoc();
    };
}

#endif //NOSQL_DB_DB_H
