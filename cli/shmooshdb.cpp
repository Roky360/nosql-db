#include <iostream>
#include "cli_manager/CLIManager.h"
#include "../utils/ioutils.h"

using namespace std;
using namespace cli;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: shmooshdb path/to/file.db";
        return 0;
    }

    string path = argv[1];
    auto *cli = new CLIManager(path);
    cli->mainLoop();

    delete cli;
    return 0;
}

