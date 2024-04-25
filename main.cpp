#include <iostream>
#include "dal/Dal.h"
#include "utils/ioutils.h"
#include "document/document.h"

using namespace std;
using namespace dal;
using namespace ioutils;

int main() {
    Options options(Options::defaultOptions);
//    options.minPageFillPercent = .0125;
//    options.maxPageFillPercent = .025;
    Dal dal("../test.db", options);

    auto d = new Document();
    d->dal = &dal;
    d->root = dal.meta->root;
    d->id = "testDoc";

//    d->put("key1", "val1");
//    d->put("key2", "val2");
//    d->put("key3", "val3");
//    d->put("key4", "val4");
//    d->put("key5", "val5");
//    d->put("key6", "val6");

//    for (int i = 1; i <= 100; i++) {
//        d->put("key" + to_string(i),"value" + to_string(i));
//    }

    auto res = d->get("key10");
    if (!res) {
        cout << "not found!" << endl;
    } else {
        cout << "key: " << res->key << ", val: " << res->value << endl;
    }

    delete d;
    dal.writeFreelist();
    dal.writeMeta();


//    Page *p = dal.allocateEmptyPage();
//    p->num = dal.getNextPageNum();
//    const char *s = "test";
//    memcpy(p->data, s, strlen(s));
//    dal.writePage(p);
//
//    delete p;
//    p = dal.allocateEmptyPage();
//    dal.fl->serialize(p->data);
//    dal.fl->maxPage = 99;
//    dal.fl->deserialize(p->data);

    return 0;
}
