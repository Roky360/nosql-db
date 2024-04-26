#include <iostream>
#include "dal/Dal.h"
#include "utils/ioutils.h"
#include "document/document.h"

using namespace std;
using namespace dal;
using namespace ioutils;

#define DELETE_FILE

int main() {
    string fileName = "../test.db";
#ifdef DELETE_FILE
    remove(fileName.data());
#endif

    Options options(Options::defaultOptions);
//    options.minPageFillPercent = .0125;
//    options.maxPageFillPercent = .025;
    Dal dal(fileName, options);

    auto d = new Document();
    d->dal = &dal;
    d->root = dal.meta->root;
    d->id = "testDoc";


    for (int i = 1; i <= 100; i++) {
        d->put("key" + to_string(i), "value" + to_string(i));
    }

    auto res = d->get("key1");
    if (!res) {
        cout << "not found!" << endl;
    } else {
        cout << "key: " << res->key << ", val: " << res->value << endl;
    }

    delete d;
    dal.writeFreelist();
    dal.writeMeta();

    return 0;
}
