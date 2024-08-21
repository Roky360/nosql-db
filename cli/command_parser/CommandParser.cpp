#include "CommandParser.h"
#include "../misc/ExecutionResult.h"
#include "../../utils/ioutils.h"

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

    Command CommandParser::parseCommand(vector<string> tokens, ExecutionResult &result) {
        // ignore empty queries
        if (tokens.empty()) {
            result = ExecutionResult(ResultStatus::OK);
            return Command{CmdType::NOOP, vector<string>()};
        }

        string cmdName = ioutils::toLower(tokens[0]);
        CmdType cmd = Command::typeFromString(cmdName);
        vector<string> args = vector<string>(tokens.begin() + 1, tokens.end());

        // try to call the right parser
        auto parserFunc = cmdToParserMap.find(cmd);
        if (parserFunc != cmdToParserMap.end()) {
            return (this->*(parserFunc->second))(args, result);
        }
        // command not found - return an error
        result = ExecutionResult(ResultStatus::ERROR, "Invalid command \"" + cmdName + "\".");
        return Command{CmdType::NOOP, vector<string>()};
    }

    Command CommandParser::parseClose(vector<string> args, ExecutionResult &result) {
        result = ExecutionResult(ResultStatus::EXIT);
        return Command{CmdType::CLOSE, vector<string>()};
    }

    Command CommandParser::parseHelp(vector<string> args, ExecutionResult &result) {
        string helpMsg = "help message\n";
        result = ExecutionResult(ResultStatus::OK, "", helpMsg);
        return Command{CmdType::HELP, vector<string>{helpMsg}};
    }

    Command CommandParser::parseCollection(vector<string> args, ExecutionResult &result) {
        if (args.empty()) {
            result = ExecutionResult(ResultStatus::ERROR, "Expected collection id.");
            return Command{CmdType::NOOP, vector<string>()};
        }
        if (!this->stateManager->isAtTopLevel()) {
            result = ExecutionResult(ResultStatus::ERROR, "A collection can be entered only from the top-level.");
            return Command{CmdType::NOOP, vector<string>()};
        }

        result = ExecutionResult(ResultStatus::OK);
        return Command{CmdType::COLLECTION, args};
    }

    Command CommandParser::parseDocument(vector<string> args, ExecutionResult &result) {
        if (args.empty()) {
            result = ExecutionResult(ResultStatus::ERROR, "Expected document id.");
            return Command{CmdType::NOOP, vector<string>()};
        }
        if (!this->stateManager->isInCollection()) {
            result = ExecutionResult(ResultStatus::ERROR, "A document can be entered only from inside a collection.");
            return Command{CmdType::NOOP, vector<string>()};
        }

        result = ExecutionResult(ResultStatus::OK);
        return Command{CmdType::DOCUMENT, args};
    }

    Command CommandParser::parseGet(vector<string> args, ExecutionResult &result) {
        if (args.empty()) {
            result = ExecutionResult(ResultStatus::ERROR, "Expected resource id.");
            return Command{CmdType::NOOP, vector<string>()};
        }
        if (!this->stateManager->isInDocument()) {
            result = ExecutionResult(ResultStatus::ERROR, "Can get resources only from inside documents.");
            return Command{CmdType::NOOP, vector<string>()};
        }

        result = ExecutionResult(ResultStatus::OK);
        return Command{CmdType::GET, args};
    }

    Command CommandParser::parsePut(vector<string> args, ExecutionResult &result) {
        if (args.size() < 2) {
            result = ExecutionResult(ResultStatus::ERROR, "Expected resource id.");
            return Command{CmdType::NOOP, vector<string>()};
        }
        if (!this->stateManager->isInDocument()) {
            result = ExecutionResult(ResultStatus::ERROR, "Must be inside a document to use the `put` command.");
            return Command{CmdType::NOOP, vector<string>()};
        }

        result = ExecutionResult(ResultStatus::OK);
        return Command{CmdType::PUT, args};
    }

    Command CommandParser::parseDelete(vector<string> args, ExecutionResult &result) {
        if (args.empty()) {
            result = ExecutionResult(ResultStatus::ERROR, "Expected resource id.");
            return Command{CmdType::NOOP, vector<string>()};
        }
        if (!(this->stateManager->isInCollection() || this->stateManager->isInDocument())) {
            result = ExecutionResult(ResultStatus::ERROR,
                                     "Must be inside a document or a collection to use the `delete` command.");
            return Command{CmdType::NOOP, vector<string>()};
        }

        result = ExecutionResult(ResultStatus::OK);
        return Command{CmdType::DELETE, args};
    }

    Command CommandParser::parseNavigateUp(vector<string> args, ExecutionResult &result) {
        result = ExecutionResult(ResultStatus::OK);
        return Command{CmdType::NAV_UP, args};
    }
} // cli