cmake_minimum_required(VERSION 3.28)
project(nosql_db)

set(CMAKE_CXX_STANDARD 23)

add_executable(nosql_db main.cpp
        dal/dal.cpp
        dal/dal.h
        dal/freelist.cpp
        dal/freelist.h
        dal/meta.cpp
        dal/meta.h
        dal/node/node.cpp
        dal/node/node.h
        db/db.cpp
        db/db.h
        db/containers/document.cpp
        db/containers/containers.h
        utils/ioutils.cpp
        utils/ioutils.h
        config/config.h
        config/options.h
        db/containers/collection.cpp
        db/containers/collection/collection.h
        config/options.cpp
        utils/logging/logging.cpp
        utils/logging/logging.h
        db/tx/tx.cpp
        db/tx/tx.h
        db/containers/container.cpp
        db/containers/container/container.h
        cli/shmooshdb.cpp
        cli/cli_manager/cli_manager.cpp
        cli/cli_manager/CLIManager.h
        cli/state_manager/DBStateManager.cpp
        cli/state_manager/DBStateManager.h
        cli/state_manager/PathEntry.h
        cli/command_parser/CommandParser.cpp
        cli/command_parser/CommandParser.h
        cli/command_dispatcher/CommandDispatcher.cpp
        cli/command_dispatcher/CommandDispatcher.h
        cli/misc/ExecutionResult.h
        cli/misc/Command.h
)
