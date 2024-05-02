#include <iostream>
#include "logging.h"

void logError(const string &msg, bool trowError) {
    if (trowError)
        throw runtime_error(msg);
    else
        cout << msg << endl;
}

void throwWriteInReadTxError() {
    logError("Trying to perform write operation in read transaction.");
}

