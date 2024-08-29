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
        NAV_TO_TOP,
    };

    static const string COMMANDS_HELP_MESSAGE = "\nNoSQL DB command list:\n"
                                                "You may chain multiple commands in the same line.\n\n"
                                                "close         - Closes the DB file and exits.\n"
                                                "exit          - \n"
                                                "help          - Displays this message.\n"
                                                "collection    - Enters a collection. Must be in top-level.\n"
                                                "col           - \n"
                                                "document      - Enters a document. Must be inside a collection. Document "
                                                "will be created if it doesn't exist.\n"
                                                "doc           - \n"
                                                "get           - Fetches a resource and displays its value, if it exists."
                                                "Must be inside a document.\n"
                                                "put           - Writes a key-value pair; Updates or creates new if it "
                                                "doesn't exist. Must be inside a document.\n"
                                                "delete        - Deletes a resource.\n"
                                                "del           - \n"
                                                "up            - Navigate up.\n"
                                                "..            - \n"
                                                "top           - Navigate to top-level.\n"
                                                "...           - \n"
                                                "\\             - \n"
                                                "\n";

    class Command {
    public:
        static CmdType typeFromString(const string &s) {
            unordered_map < string, CmdType > strToTypeMap = {
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
                    {"up",         CmdType::NAV_UP},
                    {"...",        CmdType::NAV_TO_TOP},
                    {"\\",         CmdType::NAV_TO_TOP},
                    {"top",        CmdType::NAV_TO_TOP},
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
