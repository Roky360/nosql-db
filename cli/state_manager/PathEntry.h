#ifndef NOSQL_DB_PATHENTRY_H
#define NOSQL_DB_PATHENTRY_H

#include <iostream>
#include <utility>
#include "../../db/db.h"

using namespace std;
using namespace db;

namespace cli {
    // Abstract base class
    class PathEntry {
    public:
        string name;

        explicit PathEntry(string name) : name(std::move(name)) {}

        virtual ~PathEntry() = default;
    };

    class FilePathEntry : public PathEntry {
    public:
        DB *db;

        FilePathEntry(const string &name, DB *db) : PathEntry(name), db(db) {}

        static FilePathEntry *cast(PathEntry *entry) {
            return dynamic_cast<FilePathEntry *>(entry);
        }
    };

    class CollectionPathEntry : public PathEntry {
    public:
        Collection *colRef;

        CollectionPathEntry(const string &name, Collection *colRef) : PathEntry(name), colRef(colRef) {}

        static CollectionPathEntry *cast(PathEntry *entry) {
            return dynamic_cast<CollectionPathEntry *>(entry);
        }
    };

    class DocumentPathEntry : public PathEntry {
    public:
        Document *docRef;

        DocumentPathEntry(const string &name, Document *docRef) : PathEntry(name), docRef(docRef) {}

        static DocumentPathEntry *cast(PathEntry *entry) {
            return dynamic_cast<DocumentPathEntry *>(entry);
        }
    };

} // cli

#endif //NOSQL_DB_PATHENTRY_H
