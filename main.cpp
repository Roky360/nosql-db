#include <iostream>
#include <algorithm>
#include "dal/dal.h"
#include "utils/ioutils.h"
#include "document/document.h"

using namespace std;
using namespace dal;
using namespace ioutils;

#ifndef DELETE_FILE
#define DELETE_FILE
#endif

int main() {
    string fileName = "../test.db";
#ifdef DELETE_FILE
    remove(fileName.data());
#endif

    Options options(Options::defaultOptions);
    options.minPageFillPercent = .0125;
    options.maxPageFillPercent = .025;
    Dal dal(fileName, options);

    auto d = new Document();
    d->dal = &dal;
    d->root = dal.meta->root;
    d->id = "testDoc";

    int cnt = 100;
    for (int i = 1; i <= cnt; i++) {
        d->put("key" + to_string(i), "value" + to_string(i));
    }

    for (int i = 1; i <= cnt; i+=3)
        d->remove("key" + to_string(i));

    // testing
    for (int i = 1; i <= cnt; i++) {
        string k = "key" + to_string(i);
        auto res = d->get(k);
        if (!res) {
            cout << "not found!" << endl;
        } else {
            cout << "key: " << res->key << ", val: " << res->value << endl;
        }
    }

    cout << endl;
    for (int i = 2; i <= dal.fl->maxPage; i++) {
        auto p = dal.getNode(i);
        if (p->items.empty() && dal.meta->root != p->pageNum && find(dal.fl->releasedPages.begin(), dal.fl->releasedPages.end(), p->pageNum) == dal.fl->releasedPages.end())
            cout << "Empty page " << p->pageNum << endl;
    }

    dal.writeFreelist();
    dal.writeMeta();
    delete d;

    return 0;
}
