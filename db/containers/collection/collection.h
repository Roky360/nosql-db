//#ifndef README_MD_COLLECTION_H
//#define README_MD_COLLECTION_H
//
//#include <string>
//#include "../container/container.h"
//#include "../containers.h"
//#include "../../config/config.h"
//#include "../../dal/node/node.h"
//
//using namespace std;
//
//namespace db {
//    class Container;
//    class Document;
//
////    class Collection : public Container {
////    public:
////        Collection();
////
////        Container *toContainer();
////
////        NodeItem *_get(const string &key) const override;
////
////        Container *_put(const string &key, const string &value) override;
////
////        Container *_remove(const string &key) override;
////
////        Document *_getRootDoc();
////
////        Document *_newDoc(const string &id);
////
////        /**
////         * Deletes this collection.
////         * If this collection doesn't exist in the database, it will do nothing.
////         */
////        void remove();
////
////        /**
////         * Returns a document in this collection with the provided name.
////         * If the document doesn't exist
////         * @param createIfAbsent Create a document with the given name if it doesn't exist at the time of calling this
////         * method. If this is false, nullptr will be returned.
////         */
////        Document *document(const string &name, bool createIfAbsent = true);
////
////
////    };
//}
//
//#endif //README_MD_COLLECTION_H
