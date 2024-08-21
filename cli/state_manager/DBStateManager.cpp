#include "DBStateManager.h"

namespace cli {
    DBStateManager::DBStateManager(const string &path, DB *db) {
        this->path.push_back(new FilePathEntry(path, db));
    }

    DBStateManager::~DBStateManager() {
        PathEntry *entry;
        do {
            entry = navigateUp();
        } while (entry != nullptr);
    }

    string DBStateManager::stateToString() {
        string strRep;

        for (int i = 0; i < this->path.size(); i++) {
            strRep.append(this->path[i]->name);
            if (i < this->path.size() - 1)
                strRep.append("\\");
        }

        return strRep;
    }

    PathEntry *DBStateManager::navigateUp() {
        if (this->path.size() <= 1) {
            return nullptr;
        }

        PathEntry *entry = this->path.back();
        CollectionPathEntry *colEntry;
        if ((colEntry = CollectionPathEntry::cast(entry)) != nullptr) {
            colEntry->colRef->tx->commit();
        }
        this->path.pop_back();
        return entry;
    }

    PathEntry *DBStateManager::getTopLevel() {
        if (this->path.empty()) {
            return nullptr;
        }
        return this->path.back();
    }

    void DBStateManager::enterCollection(Collection *colRef) {
        auto entry = new CollectionPathEntry(colRef->id, colRef);
        this->path.push_back(entry);
    }

    void DBStateManager::enterDocument(Document *docRef) {
        auto entry = new DocumentPathEntry(docRef->id, docRef);
        this->path.push_back(entry);
    }

    bool DBStateManager::isInCollection() {
        return dynamic_cast<CollectionPathEntry *>(this->getTopLevel()) != nullptr;
    }

    bool DBStateManager::isInDocument() {
        return dynamic_cast<DocumentPathEntry *>(this->getTopLevel()) != nullptr;
    }

    bool DBStateManager::isAtTopLevel() {
        return dynamic_cast<FilePathEntry *>(this->getTopLevel()) != nullptr;
    }
} // cli