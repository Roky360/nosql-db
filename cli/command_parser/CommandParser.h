#ifndef NOSQL_DB_COMMANDPARSER_H
#define NOSQL_DB_COMMANDPARSER_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include "../misc/ExecutionResult.h"
#include "../misc/Command.h"
#include "../state_manager/DBStateManager.h"

using namespace std;

namespace cli {
    typedef queue<string> CmdArgs;

    class CommandParser {
    private:
        unordered_map<CmdType, Command (CommandParser::*)(CmdArgs *, ExecutionResult &)> cmdToParserMap;

        DBStateManager *stateManager;

    public:
        CommandParser(DBStateManager *dbStateManager);

        vector<Command> parseCommands(CmdArgs *tokens, ExecutionResult &result);
        Command parseCommand(CmdArgs *tokens, ExecutionResult &result);

    private:
        Command parseClose(CmdArgs *args, ExecutionResult &result);
        Command parseHelp(CmdArgs *args, ExecutionResult &result);
        Command parseCollection(CmdArgs *args, ExecutionResult &result);
        Command parseDocument(CmdArgs *args, ExecutionResult &result);
        Command parseGet(CmdArgs *args, ExecutionResult &result);
        Command parsePut(CmdArgs *args, ExecutionResult &result);
        Command parseDelete(CmdArgs *args, ExecutionResult &result);
        Command parseNavigateUp(CmdArgs *args, ExecutionResult &result);


    };

} // cli

#endif //NOSQL_DB_COMMANDPARSER_H
