#include "containers.h"
#include "../../utils/logging/logging.h"

namespace db {

    Collection::Collection() : Container() {}

    Container *Collection::toContainer() {
        return dynamic_cast<Container *>(this);
    }

    void Collection::_write() const {
        auto rootC = this->tx->_getRootCollection();
        auto thisRaw = this->serialize();
        rootC->_put(thisRaw->key, thisRaw->value);
    }

    uint64 Collection::count() const {
        return this->_elemCount;
    }

    Document *Collection::_getRootDoc() {
        auto rootDoc = new Document();
        rootDoc->root = this->root;
        rootDoc->dal = this->dal;
        rootDoc->tx = this->tx;
        rootDoc->parentCollection = this;
        return rootDoc;
    }

    Document *Collection::_newDoc(const string &id) {
        auto docRoot = new Node();
        this->tx->writeNode(docRoot);

        auto doc = new Document();
        doc->id = id;
        doc->root = docRoot->pageNum;
        doc->dal = this->dal;
        doc->tx = this->tx;
        doc->parentCollection = this;

        NodeItem *docItem = doc->toContainer()->serialize();
        auto rootDoc = _getRootDoc();
        rootDoc->_put(id, docItem->value);

        return doc;
    }

    void Collection::remove() {
        if (!this->tx->hasWritePermission()) {
            throwWriteInReadTxError();
        }

        Collection *rootC = this->tx->_getRootCollection();
        rootC->toContainer()->_remove(this->id);
    }

    Document *Collection::document(const string &name, bool createIfAbsent) {
        Document *rootDoc = _getRootDoc();

        NodeItem *item = rootDoc->toContainer()->_get(name);
        // doc does not exist
        if (!item) {
            if (createIfAbsent) {
                if (!this->tx->hasWritePermission())
                    throwWriteInReadTxError();
                return _newDoc(name);
            }
            return nullptr;
        }

        auto doc = new Document();
        doc->deserialize(item);
        doc->dal = this->dal;
        doc->tx = this->tx;
        doc->parentCollection = this;

        delete rootDoc;
        return doc;
    }

    Collection *Collection::removeDoc(const string &name) {
        Document *rootDoc = _getRootDoc();
        rootDoc->_remove(name);

        this->_elemCount--;
        return this;
    }
}



