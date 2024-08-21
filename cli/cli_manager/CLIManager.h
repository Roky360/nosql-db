#ifndef NOSQL_DB_CLI_MANAGER_H
#define NOSQL_DB_CLI_MANAGER_H

#include <iostream>
#include <stack>
#include "../state_manager/DBStateManager.h"
#include "../command_parser/CommandParser.h"
#include "../command_dispatcher/CommandDispatcher.h"

using namespace std;

namespace cli {
    class CLIManager {
    public:
        string path;
        DB *db;
        DBStateManager *stateManager;
        CommandParser *parser;
        CommandDispatcher *dispatcher;

        explicit CLIManager(const string &path);

        ~CLIManager();

        void mainLoop();

    private:
        static vector<string> handleInput();
    };
}

#endif //NOSQL_DB_CLI_MANAGER_H
