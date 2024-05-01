#ifndef NOSQL_DB_TX_H
#define NOSQL_DB_TX_H

#include <map>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include "../../config/config.h"
#include "../../dal/node/node.h"
#include "../db.h"

using namespace dal;
using namespace std;

namespace db {
    class DB;

    /**
     * Represents a transaction in the database.
     * This is an abstract class that should not be instantiated.
     */
    class Tx {
    public:
        map<pgnum, Node *> dirtyNodes;  // nodes that got changed during the transaction
        vector<pgnum> pagesToDelete;    // pages (nodes) that got deleted during the transaction
        vector<pgnum> allocatedPages;   // pages (nodes) that where created during the transaction

        DB *db;

        explicit Tx(DB *db);

        virtual void commit();

        virtual void rollback();

        /**
         * Returns if this object is a WriteTx object.
         */
        bool isWriteTransaction();

        /**
         * Returns if this object is a ReadTx object.
         */
        bool isReadTransaction();
    };

    class ReadTx : public Tx {
    private:
        shared_lock<shared_mutex> lock;

    public:
        explicit ReadTx(DB *db);

        ~ReadTx();

        void commit() override;

        void rollback() override;
    };

    class WriteTx : public Tx {
    private:
        unique_lock<shared_mutex> lock;

    public:
        explicit WriteTx(DB *db);

        ~WriteTx();

        void commit() override;

        void rollback() override;
    };

    /**
     * A special transaction type used by the DB class when operations are performed not in transaction.
     * This transaction doesn't have a lock and performs operations synced.
     */
    class SharedTx : public Tx {
    public:
        explicit SharedTx(DB *db);
    };
}

#endif //NOSQL_DB_TX_H
