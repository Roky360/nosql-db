#include "tx.h"
#include "../../utils/logging/logging.h"

namespace db {
    Tx::Tx(DB *db) : db(db) {}

    void Tx::clean() {
        for (auto i: this->dirtyNodes) {
            delete i.second;
        }

        this->dirtyNodes.clear();
        this->pagesToDelete.clear();
        this->allocatedPages.clear();
    }

    void Tx::commit() {
        if (this->hasWritePermission()) {
            // write dirty nodes
            for (auto n: this->dirtyNodes) {
                this->db->dal->writeNode(n.second);
            }
            // delete the pages to delete
            for (auto p: this->pagesToDelete) {
                this->db->dal->fl->releasePage(p);
            }
            // commit freelist and meta
            this->db->dal->writeFreelist();
            this->db->dal->writeMeta();

            this->clean();
        }
    }

    void Tx::rollback() {
        if (this->hasWritePermission()) {
            // free allocated pages
            for (auto p: this->allocatedPages) {
                this->db->dal->fl->releasePage(p);
            }
            // commit freelist and meta
            this->db->dal->writeFreelist();
            this->db->dal->writeMeta();

            this->clean();
        }
    }

    bool Tx::isWriteTransaction() {
        return dynamic_cast<WriteTx *>(this) != nullptr;
    }

    bool Tx::isReadTransaction() {
        return dynamic_cast<ReadTx *>(this) != nullptr;
    }

    bool Tx::hasWritePermission() {
        return isWriteTransaction() || (dynamic_cast<SharedTx *>(this) != nullptr);
    }

    /* *** DAL Methods  *** */

    Node *Tx::getNode(pgnum pageNum) {
        Node *n = this->dirtyNodes[pageNum];
        if (n != nullptr) {
            return n;
        }
        this->dirtyNodes.erase(pageNum); // the lookup creates an element with NULL value, so we delete it

        n = this->db->dal->getNode(pageNum);
        if (!n) {
            return nullptr;
        }
        n->tx = this;
        return n;
    }

    Node *Tx::writeNode(Node *node) {
        // if this is a new node
        if (node->pageNum == 0) {
            node->pageNum = this->db->dal->getNextPageNum();
            this->allocatedPages.push_back(node->pageNum);
        }
        this->dirtyNodes[node->pageNum] = node;

        node->tx = this;
        return node;
    }

    void Tx::deleteNode(Node *node) {
        this->pagesToDelete.push_back(node->pageNum);
    }

    Collection *Tx::_getRootCollection() {
        auto rootC = new Collection();
        rootC->root = this->db->dal->meta->root;
        rootC->dal = this->db->dal;
        rootC->tx = this;
        return rootC;
    }

    Collection *Tx::createCollection(const string &name) {
        if (!this->hasWritePermission()) {
            throwWriteInReadTxError();
        }

        auto collRootNode = new Node();
        this->writeNode(collRootNode);

        auto newCollection = new Collection();
        newCollection->id = name;
        newCollection->root = collRootNode->pageNum;
        newCollection->dal = this->db->dal;
        newCollection->tx = this;

        // insert collection to the tree
        auto rootC = _getRootCollection();
        NodeItem *collItem = newCollection->serialize();
        rootC->_put(newCollection->id, collItem->value);

        delete rootC;
        return newCollection;
    }

    Collection *Tx::getCollection(const string &name, bool createIfAbsent) {
        Collection *root = _getRootCollection();
        auto item = root->_get(name);
        if (!item) {
            if (createIfAbsent) {
                Collection *c = createCollection(name);
                delete root;
                return c;
            }
            delete root;
            return nullptr;
        }
        auto c = new Collection();
        c->deserialize(item);
        c->dal = this->db->dal;
        c->tx = this;
        delete root;
        return c;
    }

    void Tx::removeCollection(const string &name) {
        Collection *root = _getRootCollection();
        root->_remove(name);
    }

    /* Read Tx */

    ReadTx::ReadTx(DB *db) : Tx(db) {
        this->lock = shared_lock(db->rwlock); // the constructor of the lock tries to lock it
    }

    ReadTx::~ReadTx() {
        if (lock.owns_lock())
            lock.unlock();
    }

    void ReadTx::commit() {
        Tx::commit();
        if (lock.owns_lock())
            this->lock.unlock();
    }

    void ReadTx::rollback() {
        Tx::rollback();
        if (lock.owns_lock())
            this->lock.unlock();
    }

    /* Write Tx */

    WriteTx::WriteTx(DB *db) : Tx(db) {
        this->lock = unique_lock(db->rwlock); // the constructor of the lock tries to lock it
    }

    WriteTx::~WriteTx() {
        if (lock.owns_lock())
            lock.unlock();
    }

    void WriteTx::commit() {
        Tx::commit();
        if (lock.owns_lock())
            this->lock.unlock();
    }

    void WriteTx::rollback() {
        Tx::rollback();
        if (lock.owns_lock())
            this->lock.unlock();
    }

    /* Shared Tx */

    SharedTx::SharedTx(DB *db) : Tx(db) {}
}
