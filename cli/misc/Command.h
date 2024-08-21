#ifndef NOSQL_DB_COMMAND_H
#define NOSQL_DB_COMMAND_H

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

namespace cli {
    enum CmdType {
        NOOP,
        CLOSE,
        HELP,
        COLLECTION,
        DOCUMENT,
        GET,
        PUT,
        DELETE,
        NAV_UP,
    };

    class Command {
    public:
        static CmdType typeFromString(const string &s) {
            unordered_map<string, CmdType> strToTypeMap = {
                    {"close",      CmdType::CLOSE},
                    {"exit",       CmdType::CLOSE},
                    {"help",       CmdType::HELP},
                    {"collection", CmdType::COLLECTION},
                    {"col",        CmdType::COLLECTION},
                    {"document",   CmdType::DOCUMENT},
                    {"doc",        CmdType::DOCUMENT},
                    {"get",        CmdType::GET},
                    {"put",        CmdType::PUT},
                    {"delete",     CmdType::DELETE},
                    {"del",        CmdType::DELETE},
                    {"..",         CmdType::NAV_UP},
            };
            return strToTypeMap[s];
        }

        CmdType type;
        vector<string> args;

        Command(CmdType type, const vector<string> &args) : type(type), args(args) {}

        Command(const string &name, const vector<string> &args) : type(typeFromString(name)), args(args) {}
    };

} // cli

#endif //NOSQL_DB_COMMAND_H
