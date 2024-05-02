#ifndef NOSQL_DB_LOGGING_H
#define NOSQL_DB_LOGGING_H

#include <string>

using namespace std;

void logError(const string &msg, bool trowError= true);

void throwWriteInReadTxError();

#endif //NOSQL_DB_LOGGING_H
