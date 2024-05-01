#include "tx.h"

namespace db {
    Tx::Tx(DB *db) : db(db) {}

    void Tx::commit() {
        if (!this->isReadTransaction()) {
            // write dirty nodes
            for (auto n: this->dirtyNodes) {
                this->db->dal->writeNode(n.second);
            }
            // delete the pages to delete
            for (auto p: this->pagesToDelete) {
                this->db->dal->fl->releasePage(p);
            }
            // commit freelist
            this->db->dal->writeFreelist();

            this->dirtyNodes.clear();
            this->pagesToDelete.clear();
            this->allocatedPages.clear();
        }
    }

    void Tx::rollback() {
        if (!this->isReadTransaction()) {
            // free allocated pages
            for (auto p: this->allocatedPages) {
                this->db->dal->fl->releasePage(p);
            }

            this->dirtyNodes.clear();
            this->pagesToDelete.clear();
            this->allocatedPages.clear();
        }
    }

    bool Tx::isWriteTransaction() {
        return dynamic_cast<WriteTx *>(this) != nullptr;
    }

    bool Tx::isReadTransaction() {
        return dynamic_cast<ReadTx *>(this) != nullptr;
    }

    /* Read Tx */

    ReadTx::ReadTx(DB *db) : Tx(db) {
        this->lock = shared_lock(db->rwlock);
        this->lock.lock();
    }

    ReadTx::~ReadTx() {
        lock.unlock();
    }

    void ReadTx::commit() {
        Tx::commit();
        this->lock.unlock();
    }

    void ReadTx::rollback() {
        Tx::rollback();
        this->lock.unlock();
    }

    /* Write Tx */

    WriteTx::WriteTx(DB *db) : Tx(db) {
        this->lock = unique_lock(db->rwlock);
        this->lock.lock();
    }

    WriteTx::~WriteTx() {
        lock.unlock();
    }

    void WriteTx::commit() {
        Tx::commit();
        this->lock.unlock();
    }

    void WriteTx::rollback() {
        Tx::rollback();
        this->lock.unlock();
    }

    /* Shared Tx */

    SharedTx::SharedTx(DB *db) : Tx(db) {}
}
