#ifndef NOSQL_DB_DBSTATEMANAGER_H
#define NOSQL_DB_DBSTATEMANAGER_H

#include <iostream>
#include "PathEntry.h"

using namespace std;

namespace cli {
    class DBStateManager {
        vector<PathEntry *> path;

    public:
        DBStateManager(const string &path, DB *db);

        virtual ~DBStateManager();

        string stateToString();

        PathEntry *navigateUp();

        PathEntry *getTopLevel();

        void enterCollection(Collection *colRef);

        void enterDocument(Document *docRef);

        bool isInCollection();
        bool isInDocument();
        bool isAtTopLevel();
    };

} // cli

#endif //NOSQL_DB_DBSTATEMANAGER_H
