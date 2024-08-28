#include "CommandParser.h"
#include "../misc/ExecutionResult.h"
#include "../../utils/ioutils.h"
#include "../cli_manager/CLIManager.h"

using namespace ioutils;

namespace cli {
    CommandParser::CommandParser(DBStateManager *dbStateManager) : stateManager(dbStateManager) {
        cmdToParserMap = {
                {CLOSE,      &CommandParser::parseClose},
                {HELP,       &CommandParser::parseHelp},
                {COLLECTION, &CommandParser::parseCollection},
                {DOCUMENT,   &CommandParser::parseDocument},
                {GET,        &CommandParser::parseGet},
                {PUT,        &CommandParser::parsePut},
                {DELETE,     &CommandParser::parseDelete},
                {NAV_UP,     &CommandParser::parseNavigateUp},
        };
    }

    vector<Command> CommandParser::parseCommands(CmdArgs *tokens, ExecutionResult &result) {
        vector<Command> commands;
        result = ExecutionResult(ResultStatus::OK);

        while (!tokens->empty() && result.status == ResultStatus::OK) {
            Command cmd = parseCommand(tokens, result);
            commands.push_back(cmd);
        }

        return commands;
    }

    Command CommandParser::parseCommand(CmdArgs *tokens, ExecutionResult &result) {
        // ignore empty queries
        if (tokens->empty()) {
            result = ExecutionResult(ResultStatus::OK);
            return Command{CmdType::NOOP, {}};
        }

        string cmdName = ioutils::toLower(tokens->front());
        tokens->pop();
        CmdType cmd = Command::typeFromString(cmdName);

        // try to call the right parser
        auto parserFunc = cmdToParserMap.find(cmd);
        if (parserFunc != cmdToParserMap.end()) {
            return (this->*(parserFunc->second))(tokens, result);
        }
        // command not found - return an error
        result = ExecutionResult(ResultStatus::ERROR, "Invalid command \"" + cmdName + "\".");
        return Command{CmdType::NOOP, {}};
    }

    Command CommandParser::parseClose(CmdArgs *args, ExecutionResult &result) {
        result = ExecutionResult(ResultStatus::EXIT);
        return Command{CmdType::CLOSE, {}};
    }

    Command CommandParser::parseHelp(CmdArgs *args, ExecutionResult &result) {
        string helpMsg = "help message\n";
        result = ExecutionResult(ResultStatus::OK, "", helpMsg);
        return Command{CmdType::HELP, {helpMsg}};
    }

    Command CommandParser::parseCollection(CmdArgs *args, ExecutionResult &result) {
        if (args->empty()) {
            result = ExecutionResult(ResultStatus::ERROR, "Expected collection id.");
            return Command{CmdType::NOOP, {}};
        }
        if (!this->stateManager->isAtTopLevel()) {
            result = ExecutionResult(ResultStatus::ERROR, "A collection can be entered only from the top-level.");
            return Command{CmdType::NOOP, {}};
        }

        result = ExecutionResult(ResultStatus::OK);
        auto colId = args->front();
        args->pop();
        return Command{CmdType::COLLECTION, {colId}};
    }

    Command CommandParser::parseDocument(CmdArgs *args, ExecutionResult &result) {
        if (args->empty()) {
            result = ExecutionResult(ResultStatus::ERROR, "Expected document id.");
            return Command{CmdType::NOOP, {}};
        }
        if (!this->stateManager->isInCollection()) {
            result = ExecutionResult(ResultStatus::ERROR, "A document can be entered only from inside a collection.");
            return Command{CmdType::NOOP, {}};
        }

        result = ExecutionResult(ResultStatus::OK);
        auto docId = args->front();
        args->pop();
        return Command{CmdType::DOCUMENT, {docId}};
    }

    Command CommandParser::parseGet(CmdArgs *args, ExecutionResult &result) {
        if (args->empty()) {
            result = ExecutionResult(ResultStatus::ERROR, "Expected resource id.");
            return Command{CmdType::NOOP, {}};
        }
        if (!this->stateManager->isInDocument()) {
            result = ExecutionResult(ResultStatus::ERROR, "Can get resources only from inside documents.");
            return Command{CmdType::NOOP, {}};
        }

        result = ExecutionResult(ResultStatus::OK);
        auto resourceId = args->front();
        args->pop();
        return Command{CmdType::GET, {resourceId}};
    }

    Command CommandParser::parsePut(CmdArgs *args, ExecutionResult &result) {
        if (args->size() < 2) {
            result = ExecutionResult(ResultStatus::ERROR, "Expected resource id.");
            return Command{CmdType::NOOP, vector<string>()};
        }
        if (!this->stateManager->isInDocument()) {
            result = ExecutionResult(ResultStatus::ERROR, "Must be inside a document to use the `put` command.");
            return Command{CmdType::NOOP, {}};
        }

        result = ExecutionResult(ResultStatus::OK);
        auto key = args->front();
        args->pop();
        auto val = args->front();
        args->pop();
        return Command{CmdType::PUT, {key, val}};
    }

    Command CommandParser::parseDelete(CmdArgs *args, ExecutionResult &result) {
        if (args->empty()) {
            result = ExecutionResult(ResultStatus::ERROR, "Expected resource id.");
            return Command{CmdType::NOOP, vector<string>()};
        }
        if (!(this->stateManager->isInCollection() || this->stateManager->isInDocument())) {
            result = ExecutionResult(ResultStatus::ERROR,
                                     "Must be inside a document or a collection to use the `delete` command.");
            return Command{CmdType::NOOP, {}};
        }

        result = ExecutionResult(ResultStatus::OK);
        auto resourceId = args->front();
        args->pop();
        return Command{CmdType::DELETE, vector<string>{resourceId}};
    }

    Command CommandParser::parseNavigateUp(CmdArgs *args, ExecutionResult &result) {
        result = ExecutionResult(ResultStatus::OK);
        return Command{CmdType::NAV_UP, {}};
    }
} // cli