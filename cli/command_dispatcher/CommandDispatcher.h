#ifndef NOSQL_DB_COMMANDDISPATCHER_H
#define NOSQL_DB_COMMANDDISPATCHER_H

#include "../../db/db.h"
#include "../state_manager/DBStateManager.h"
#include "../misc/ExecutionResult.h"
#include "../misc/Command.h"

namespace cli {
    class CommandDispatcher {
    private:
        unordered_map<CmdType, ExecutionResult (CommandDispatcher::*)(const Command &)> cmdToExeMap;

        DBStateManager *stateManager;

    public:
        explicit CommandDispatcher(DBStateManager *stateManager);

        ExecutionResult execute(const Command& cmd);
        ExecutionResult executeNoop(const Command& cmd);
        ExecutionResult executeClose(const Command& cmd);
        ExecutionResult executeHelp(const Command& cmd);
        ExecutionResult executeCollection(const Command& cmd);
        ExecutionResult executeDocument(const Command& cmd);
        ExecutionResult executeGet(const Command& cmd);
        ExecutionResult executePut(const Command& cmd);
        ExecutionResult executeDelete(const Command& cmd);
        ExecutionResult executeNavigateUp(const Command& cmd);
    };

} // cli

#endif //NOSQL_DB_COMMANDDISPATCHER_H
