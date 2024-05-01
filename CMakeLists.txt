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
        containers/document/document.cpp
        containers/document/document.h
        utils/ioutils.cpp
        utils/ioutils.h
        config/config.h
        config/options.h
        containers/collection/collection.cpp
        containers/collection/collection.h
        config/options.cpp
        utils/logging/logging.cpp
        utils/logging/logging.h
        db/tx/tx.cpp
        db/tx/tx.h
        containers/container/container.cpp
        containers/container/container.h)
