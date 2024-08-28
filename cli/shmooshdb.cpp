#include <iostream>
#include "cli_manager/CLIManager.h"
#include "../utils/ioutils.h"

using namespace std;
using namespace cli;

void f(vector<int> a) {
    a.pop_back();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: shmooshdb path/to/file.db";
        return 0;
    }

    vector<int> a1;
    a1.push_back(1);
    a1.push_back(2);
    f(a1);

    string path = argv[1];
    auto *cli = new CLIManager(path);
    cli->mainLoop();

    delete cli;
    return 0;
}

