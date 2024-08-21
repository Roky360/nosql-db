#include <sstream>
#include "CLIManager.h"
#include "../../utils/ioutils.h"
#include "../misc/ExecutionResult.h"

using namespace ioutils;

namespace cli {
    CLIManager::CLIManager(const string &path) {
        this->path = path;
        this->db = new DB();
        this->db->open(path);
        this->stateManager = new DBStateManager(ioutils::split(this->path, "/").back(), this->db);
        this->parser = new CommandParser(this->stateManager);
        this->dispatcher = new CommandDispatcher(this->stateManager);
    }

    CLIManager::~CLIManager() {
        delete this->parser;
        delete this->dispatcher;
        delete this->stateManager;
        delete this->db;
    }

    void CLIManager::mainLoop() {
        bool active = true;

        while (active) {
            cout << this->stateManager->stateToString() << " >> ";

            vector<string> tokens = handleInput();
            ExecutionResult result;
            Command cmd = this->parser->parseCommand(tokens, result);

            if (result.status == ERROR) {
                cout << result.message << endl;
            } else {
                // all good, execute the command
                result = this->dispatcher->execute(cmd);

                switch (result.status) {
                    case OK:
                    case DELETED:
                    case CREATED:
                    case NOT_FOUND:
                        if (!result.message.empty()) {
                            cout << result.message << endl;
                        }
                        if (!result.result.empty()) {
                            cout << result.result << endl;
                        }
                        break;
                    case ERROR:
                        cout << result.message << endl;
                        break;
                    case FATAL:
                        cout << result.message << endl;
                        active = false;
                        break;
                    case EXIT:
                        active = false;
                        break;
                }
            }
        }
    }

    vector<string> CLIManager::handleInput() {
        vector<string> tokens;
        string line;
        string token;
        std::getline(cin, line);
        stringstream stream(line);
        while (stream >> token) {
            tokens.push_back(token);
        }

        // remove empty elements
        int i = 0;
        for (vector<string> copy(tokens); i < copy.size(); i++) {
            if (copy[i].empty() || copy[i] == " " || copy[i] == "\t") {
                tokens.erase(tokens.begin() + i);
            }
        }

        return tokens;
    }
}
