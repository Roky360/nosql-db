#ifndef NOSQL_DB_OPTIONS_H
#define NOSQL_DB_OPTIONS_H

#include <string>
#include "config.h"

using namespace std;

class Options {
public:
    int pageSize;
    float minPageFillPercent;
    float maxPageFillPercent;
    string dbFileExtension; // may be removed ?

    explicit Options(
            int pageSize = defaultOptions.pageSize,
            float minPageFillPercent = defaultOptions.minPageFillPercent,
            float maxPageFillPercent = defaultOptions.maxPageFillPercent,
            string dbFileExtension = defaultOptions.dbFileExtension
    );

    Options(Options &o);

    static Options defaultOptions;
};

#endif //NOSQL_DB_OPTIONS_H
