#include "Dal.h"
#include <iostream>
#include <cstdarg>

using namespace std;

namespace dal {
    Dal::Dal(const string &path, Options &options) : pageSize(options.pageSize), fl(new FreeList()),
                                                     meta(new Meta()), options(options) {
        file.open(path, ios::in | ios::out | ios::binary);
        // file not exist, create new one
        if (!file.is_open()) {
            // create file
            file.open(path, ios::out | ios::binary);
            if (!file.is_open()) {
                cerr << "Unable to create file at \"" << path << '"' << endl;
                exit(1);
            }
            file.close();
            file.open(path, ios::in | ios::out | ios::binary);

            if (!file.is_open()) {
                cerr << "Unable to open file at \"" << path << '"' << endl;
                exit(1);
            }

            this->meta->freelistPage = getNextPageNum();
            Node rootNode;
            if (!writeNode(&rootNode)) {
                cerr << "Unable to create root node." << endl;
                exit(1);
            }
            this->meta->root = rootNode.pageNum;
            if (!writeMeta()) {
                cerr << "Unable to write meta." << endl;
                exit(1);
            }
            if (!writeFreelist()) {
                cerr << "Unable to write freelist." << endl;
                exit(1);
            }
        } else {
            // file exists
            if (!readMeta()) {
                cerr << "Unable to read meta." << endl;
                exit(1);
            }
            if (!readFreelist()) {
                cerr << "Unable to read freelist." << endl;
                exit(1);
            }
        }
    }

    Dal::~Dal() {
        if (file.is_open()) {
            file.close();
        }
        delete this->fl;
        delete this->meta;
    }

    Page::~Page() {
        delete[] this->data;
    }

    Page *Dal::allocateEmptyPage() const {
        Page *page = new Page();
        page->data = new char[this->pageSize]{0};
        return page;
    }

    bool Dal::writePage(Page *page) {
        unsigned long offset = page->num * this->pageSize;
        this->file.seekp(offset);
        this->file.write(page->data, this->pageSize);
//        this->file << page->data;
        return file.good();
    }

    Page *Dal::readPage(pgnum pgnum) {
        Page *page = this->allocateEmptyPage();
        page->num = pgnum;
        unsigned long offset = this->pageSize * pgnum;

        this->file.seekg(offset);
        this->file.read(page->data, this->pageSize);

        if (file.good())
            return page;
        return nullptr;
    }

    pgnum Dal::getNextPageNum() const {
        return this->fl->getNextPageNum();
    }

    bool Dal::writeMeta() {
        Page *p = allocateEmptyPage();
        p->num = META_PAGE_NUM;
        this->meta->serialize(p->data);
        bool success = writePage(p);
        delete p;
        return success;
    }

    bool Dal::readMeta() {
        Page *p = readPage(META_PAGE_NUM);
        if (!p) {
            delete p;
            return false;
        }

        this->meta = new Meta();
        this->meta->deserialize(p->data);
        delete p;
        return true;
    }

    bool Dal::writeFreelist() {
        Page *p = allocateEmptyPage();
        p->num = this->meta->freelistPage;
        this->fl->serialize(p->data);
        bool success = this->writePage(p);

        delete p;
        return success;
    }

    bool Dal::readFreelist() {
        Page *p = readPage(this->meta->freelistPage);
        if (!p) {
            delete p;
            return false;
        }

        this->fl = new FreeList();
        this->fl->deserialize(p->data);
        delete p;
        return true;
    }

    Node *Dal::getNode(pgnum pageNum) {
        Page *p = readPage(pageNum);
        if (!p) {
            return nullptr;
        }
        Node *n = new Node();
        n->deserialize(p->data);
        n->pageNum = pageNum;
        n->dal = this;
        delete p;
        return n;
    }

    vector<Node *> Dal::getNodes(Node *root, const vector<int> &indexes) {
        vector<Node *> nodes = {root};
        Node *currNode = root;
        for (int i: indexes) {
            currNode = this->getNode(currNode->childNodes[i]);
            if (!currNode) {
                // TODO: THROW IO ERROR
                cerr << "Uh error" << endl;
                exit(1);
            }
            nodes.push_back(currNode);
        }

        return nodes;
    }

    bool Dal::writeNode(Node *node) {
        Page *p = allocateEmptyPage();
        // if the node's page num not initialized - doesn't exist on disk
        if (node->pageNum == 0) {
            node->pageNum = getNextPageNum();
        }
        p->num = node->pageNum;

        node->serialize(p->data, this->pageSize);
        bool success = writePage(p);
        delete p;
        return success;
    }

    bool Dal::writeNodes(Node *first, ...) {
        va_list args;
        va_start(args, first);
        bool success = __writeNodes(first, args);
        va_end(args);
        return success;
    }

    bool Dal::__writeNodes(Node *first, va_list &args) {
        Node *n = first;
        bool success = true;

        while (n != nullptr) {
            success = success && writeNode(n);
            n = va_arg(args, Node *);
        }

        return success;
    }

    void Dal::deleteNode(Node *node) const {
        this->fl->releasePage(node->pageNum);
    }

    float Dal::nodeMaxThreshold() const {
        return (float) this->pageSize * this->options.maxPageFillPercent;
    }

    float Dal::nodeMinThreshold() const {
        return (float) this->pageSize * this->options.minPageFillPercent;
    }

    bool Dal::isNodeOverPopulated(Node *node) const {
        return (float) node->size() > nodeMaxThreshold();
    }

    bool Dal::isNodeUnderPopulated(Node *node) const {
        return (float) node->size() < nodeMinThreshold();
    }

    int Dal::getSplitIndex(Node *node) {
        int size = 0;
        size += NODE_HEADER_SIZE;

        for (int i = 0; i < node->items.size(); i++) {
            size += node->itemSize(i);
            if ((float) size >= nodeMinThreshold() && i < node->items.size() - 1) {
                return i + 1;
            }
        }
        return -1;
    }


} // dal