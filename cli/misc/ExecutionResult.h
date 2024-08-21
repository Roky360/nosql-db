#ifndef NOSQL_DB_EXECUTIONRESULT_H
#define NOSQL_DB_EXECUTIONRESULT_H

#include <iostream>
#include <string>

using namespace std;

namespace cli {
    enum ResultStatus {
        OK,
        CREATED,
        DELETED,
        NOT_FOUND,
        ERROR,
        FATAL,
        EXIT,
    };

    class ExecutionResult {
    public:
        ResultStatus status;
        string message;
        string result; // if this is not empty, will be printed to console

        ExecutionResult() = default;

        explicit ExecutionResult(ResultStatus status, const string &message = "", const string &result = "")
                : status(status), message(string(message)), result(string(result)) {}
    };

} // cli

#endif //NOSQL_DB_EXECUTIONRESULT_H
