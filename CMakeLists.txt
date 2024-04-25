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
        document/document.cpp
        document/document.h
        utils/ioutils.cpp
        utils/ioutils.h
        config/config.h
        config/options.h
        collection/collection.cpp
        collection/collection.h
        config/options.cpp)
