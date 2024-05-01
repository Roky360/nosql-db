#ifndef NOSQL_DB_CONTAINER_H
#define NOSQL_DB_CONTAINER_H

#include "../../dal/node/node.h"

using namespace dal;

/**
 * Container is the base class of Collections and Documents.
 * It is an abstract class that should not be instantiated.
 */
class Container {
public:
    string id;
    pgnum root;

    Dal *dal;

    Container();

    virtual NodeItem *get(const string &key) const;

    virtual Container *put(const string &key, const string &value);

    virtual Container *remove(const string &key);
};

#endif //NOSQL_DB_CONTAINER_H
