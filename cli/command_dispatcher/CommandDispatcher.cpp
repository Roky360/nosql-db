#include "CommandDispatcher.h"

namespace cli {
    CommandDispatcher::CommandDispatcher(DBStateManager *stateManager) : stateManager(stateManager) {
        cmdToExeMap = {
                {NOOP,       &CommandDispatcher::executeNoop},
                {CLOSE,      &CommandDispatcher::executeClose},
                {HELP,       &CommandDispatcher::executeHelp},
                {COLLECTION, &CommandDispatcher::executeCollection},
                {DOCUMENT,   &CommandDispatcher::executeDocument},
                {GET,        &CommandDispatcher::executeGet},
                {PUT,        &CommandDispatcher::executePut},
                {DELETE,     &CommandDispatcher::executeDelete},
                {NAV_UP,     &CommandDispatcher::executeNavigateUp},
        };
    }

    ExecutionResult CommandDispatcher::execute(const Command &cmd) {
        auto executorFunc = cmdToExeMap.find(cmd.type);
        if (executorFunc == cmdToExeMap.end()) {
            return ExecutionResult{ResultStatus::ERROR, "Invalid command."};
        }

        return (this->*(executorFunc->second))(cmd);
    }

    ExecutionResult CommandDispatcher::executeNoop(const Command &cmd) {
        return ExecutionResult{ResultStatus::OK};
    }

    ExecutionResult CommandDispatcher::executeClose(const Command &cmd) {
        return ExecutionResult{ResultStatus::EXIT};
    }

    ExecutionResult CommandDispatcher::executeHelp(const Command &cmd) {
        return ExecutionResult{ResultStatus::OK, cmd.args[0]};
    }

    ExecutionResult CommandDispatcher::executeCollection(const Command &cmd) {
        auto entry = FilePathEntry::cast(this->stateManager->getTopLevel());
        DB *db = entry->db;

        auto tx = db->writeTransaction();
        auto col = tx->getCollection(cmd.args[0]);
        this->stateManager->enterCollection(col);

        return ExecutionResult{ResultStatus::OK};
    }

    ExecutionResult CommandDispatcher::executeDocument(const Command &cmd) {
        auto entry = CollectionPathEntry::cast(this->stateManager->getTopLevel());
        Collection *col = entry->colRef;

        auto doc = col->document(cmd.args[0]);
        this->stateManager->enterDocument(doc);

        return ExecutionResult{ResultStatus::OK};
    }

    ExecutionResult CommandDispatcher::executeGet(const Command &cmd) {
        auto entry = DocumentPathEntry::cast(this->stateManager->getTopLevel());
        auto doc = entry->docRef;

        NodeItem *item = doc->get(cmd.args[0]);
        if (item == nullptr) {
            return ExecutionResult{ResultStatus::NOT_FOUND,
                                   "The item `" + cmd.args[0] + "` does not exist in `" + doc->id + "`."};
        }

        return ExecutionResult{ResultStatus::OK, "", item->value};
    }

    ExecutionResult CommandDispatcher::executePut(const Command &cmd) {
        auto entry = DocumentPathEntry::cast(this->stateManager->getTopLevel());
        auto doc = entry->docRef;

        doc->put(cmd.args[0], cmd.args[1]);

        return ExecutionResult{ResultStatus::OK};
    }

    ExecutionResult CommandDispatcher::executeDelete(const Command &cmd) {
        auto entry = this->stateManager->getTopLevel();
        Container *container;
        CollectionPathEntry *colEntry;
        DocumentPathEntry *docEntry;
        if ((colEntry = CollectionPathEntry::cast(entry)) != nullptr) {
            container = colEntry->colRef->toContainer();
        } else if ((docEntry = DocumentPathEntry::cast(entry)) != nullptr) {
            container = docEntry->docRef->toContainer();
        } else {
            return ExecutionResult{ResultStatus::NOT_FOUND, "Not in a collection nor in a document."};
        }

        // if no argument provided, delete the resource itself (document/collection)
        if (cmd.args.empty()) {
            container->remove();
        } else {
            container->_remove(cmd.args[0]);
        }

        return ExecutionResult{ResultStatus::DELETED};
    }

    ExecutionResult CommandDispatcher::executeNavigateUp(const Command &cmd) {
        auto newEntry = this->stateManager->navigateUp();
        // if this is already the top level
        if (newEntry == nullptr) {
            return ExecutionResult{ResultStatus::NOT_FOUND};
        }
        return ExecutionResult{ResultStatus::OK};
    }


} // cli