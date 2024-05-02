#ifndef NOSQL_DB_DB_H
#define NOSQL_DB_DB_H

#include <shared_mutex>
#include "../dal/Dal.h"
#include "tx/tx.h"
#include "../config/options.h"

using namespace std;
using namespace dal;

namespace dal {
    class Dal;
}

namespace db {
    class Tx;
    class ReadTx;
    class WriteTx;
    class SharedTx;

    class DB {
    private:
        SharedTx *defaultTx;
    public:
        Dal *dal;
        shared_mutex rwlock; // read-write lock for transactions

        DB();

        ~DB();

        void open(const string &path, Options& options=Options::defaultOptions);

        void close();

        /* Transactions */
        /**
         * Opens a read transaction and returns it. All operations must be performed on the transaction itself.
         * Once commit or rollback are performed, the lock is released.
         * @note It is not allowed to perform write operations on read transactions.
         */
        ReadTx *readTransaction();

        /**
         * Opens a write transaction and returns it. All operations must be performed on the transaction itself.
         * Once commit or rollback are performed, the lock is released.
         */
        WriteTx *writeTransaction();

        // planning of needed functions
//        string createCollection(const string &name="");
//        Collection *getCollection(const string &name); // maybe replace with CollectionSnapshot object
//        void deleteCollection(const string &name);
//
//        // on Collection snapshot class
//        Document getDocument(const string &id); // replace with DocumentSnapshot object
//        vector<Document> where(); // _get all documents that satisfy a certain condition
//        Collection* deleteDocument(const string &id);
//        void deleteCollection(); // deletes the collection itself
//
//        // on DocumentSnapshot
//        Document* get(); // returns the entire doc as DocumentSnapshot
//        string get(const string &key); // returns a value from given key. * more data types will be added
//        Document* put(const string &key, const string &value); // puts key-value in `doc`; overrides data if `key` exists.
//        void deleteDoc();
    };
}

#endif //NOSQL_DB_DB_H
