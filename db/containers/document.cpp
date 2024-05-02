#include "containers.h"

namespace db {
    Document::Document() : Container(), parentCollection(nullptr) {}

    Container *Document::toContainer() {
        return dynamic_cast<Container *>(this);
    }

    void Document::remove() {
        if (this->parentCollection)
            this->parentCollection->removeDoc(this->id);
    }

    NodeItem *Document::get(const string &key) {
        auto item = this->_get(key);
        return item;
    }

    Document *Document::put(const string &key, const string &value) {
        this->_put(key, value);
        return this;
    }

    Document *Document::remove(const string &key) {
        this->_remove(key);
        return this;
    }
}



