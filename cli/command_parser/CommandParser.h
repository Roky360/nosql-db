#ifndef NOSQL_DB_COMMANDPARSER_H
#define NOSQL_DB_COMMANDPARSER_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include "../misc/ExecutionResult.h"
#include "../misc/Command.h"
#include "../state_manager/DBStateManager.h"

using namespace std;

namespace cli {
    class CommandParser {
    private:
        unordered_map<CmdType, Command (CommandParser::*)(vector<string>, ExecutionResult &)> cmdToParserMap;

        DBStateManager *stateManager;

    public:
        CommandParser(DBStateManager *dbStateManager);

        Command parseCommand(vector<string> tokens, ExecutionResult &result);

        Command parseClose(vector<string> args, ExecutionResult &result);
        Command parseHelp(vector<string> args, ExecutionResult &result);
        Command parseCollection(vector<string> args, ExecutionResult &result);
        Command parseDocument(vector<string> args, ExecutionResult &result);
        Command parseGet(vector<string> args, ExecutionResult &result);
        Command parsePut(vector<string> args, ExecutionResult &result);
        Command parseDelete(vector<string> args, ExecutionResult &result);
        Command parseNavigateUp(vector<string> args, ExecutionResult &result);


    };

} // cli

#endif //NOSQL_DB_COMMANDPARSER_H
