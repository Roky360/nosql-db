#include <iostream>
#include "db/db.h"

#include "utils/ioutils.h"
using namespace std;
using namespace db;

using namespace std;
using namespace ioutils;

#ifndef DELETE_FILE
//#define DELETE_FILE
#endif

//int main() {
//    string fileName = "../test.db";
//#ifdef DELETE_FILE
//    remove(fileName.data());
//#endif
//    Options options(Options::defaultOptions);
//    options.minPageFillPercent = .0125;
//    options.maxPageFillPercent = .025;
//    DB db;
//    db.open(fileName, options);
//
//    WriteTx *tx = db.writeTransaction();
//    Collection *c = tx->getCollection("testCollection");
//    auto d = c->document("doc1");
//
//    for (int i = 1; i <= 5; i++)
//        d->put("key" + to_string(i), "value" + to_string(i));
//    d->remove("key26");
//
//    tx->commit();
//
//    tx = db.writeTransaction();
//    auto c2 = tx->getCollection("testCollection");
//    auto d2 = c2->document("doc2");
//    d2->put("key1", "value1");
//    tx->commit();
//
//    auto item = d2->get("key1");
//    if (!item) {
//        cout << "note found" << endl;
//    } else {
//        cout << "val is: " << item->value << endl;
//    }
//
//    db.close();
//
//    return 0;
//}
