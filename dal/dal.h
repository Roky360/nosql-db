#ifndef NOSQL_DB_DAL_H
#define NOSQL_DB_DAL_H

#include <iostream>
#include <fstream>
#include <vector>
#include "../config/options.h"

#include "../config/config.h"
#include "freelist.h"
#include "meta.h"
#include "node/node.h"

using namespace std;

namespace dal {
    class Node;

    class Page {
    public:
        // page number
        pgnum num;
        char *data;

        ~Page();
    };

    class Dal {
    public:
        fstream file;
        int pageSize;
        FreeList *fl;
        Meta *meta;
        Options options;

        explicit Dal(const string &path, Options &options = Options::defaultOptions);

        ~Dal();

        Page *allocateEmptyPage() const;

        bool writePage(Page *page);

        Page *readPage(pgnum pgnum);

        pgnum getNextPageNum() const;

        bool writeMeta();

        bool readMeta();

        bool writeFreelist();

        bool readFreelist();

        Node *getNode(pgnum pageNum);

        vector<Node *> getNodes(Node *root, const vector<int> &indexes);

        bool writeNode(Node *node);

        bool writeNodes(Node *first, ...);

        bool __writeNodes(Node *first, va_list &args);

        void deleteNode(Node *node) const;

        float nodeMaxThreshold() const;

        float nodeMinThreshold() const;

        bool isNodeOverPopulated(Node *node) const;

        bool isNodeUnderPopulated(Node *node) const;

        /**
         * Returns the index in which `node` has enough children so its size is between the min and max threshold.
         * Else returns -1;
         */
        int getSplitIndex(Node *node);
    };
} // dal

#endif //NOSQL_DB_DAL_H
