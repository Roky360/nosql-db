//#ifndef NOSQL_DB_CONTAINER_H
//#define NOSQL_DB_CONTAINER_H
//
//#include "../../../dal/dal.h"
//#include "../../tx/tx.h"
//#include "../../../dal/node/node.h"
//
//using namespace dal;
//
//namespace dal {
//    class Node;
//    class NodeItem;
//    class Dal;
//}
//
///**
// * Container is the base class of Collections and Documents.
// * It is an abstract class that should not be instantiated.
// */
// namespace db {
//     class Tx;
//
////     class Container {
////     public:
////         string id;
////         pgnum root;
////
////         Dal *dal;
////         Tx *tx; // associated transaction
////
////         Container();
////
////         virtual NodeItem *serialize() const;
////
////         virtual void deserialize(NodeItem *item);
////
////         /**
////          * Get an item in a document from given key.
////          */
////         virtual NodeItem *_get(const string &key) const;
////
////         /**
////          * Writes a key-value pair to this document.
////          * If it exists it will be overwritten.
////          * @return This document
////          */
////         virtual Container *_put(const string &key, const string &value);
////
////         /**
////          * Removes a key-value pair from the document.
////          * @param key The key of the item to _remove.
////          * @return This document
////          */
////         virtual Container *_remove(const string &key);
////
////         vector<Node *> _getNodes(const vector<int> &indexes);
////     };
// }
//
//#endif //NOSQL_DB_CONTAINER_H
