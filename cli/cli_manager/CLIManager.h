#ifndef NOSQL_DB_CLI_MANAGER_H
#define NOSQL_DB_CLI_MANAGER_H

#include <iostream>
#include <queue>
#include "../state_manager/DBStateManager.h"
#include "../command_parser/CommandParser.h"
#include "../command_dispatcher/CommandDispatcher.h"

using namespace std;

namespace cli {
    typedef queue<string> CmdArgs;

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
        static CmdArgs handleInput();
    };
}

#endif //NOSQL_DB_CLI_MANAGER_H
