#ifndef NOSQL_DB_NODE_H
#define NOSQL_DB_NODE_H

#include "../../config/config.h"
#include "../Dal.h"
#include "../../db/tx/tx.h"

namespace db {
    class Tx;
}

using namespace db;

namespace dal {
    class Dal;

    class NodeItem {
    public:
        string key;
        string value;

        NodeItem();

        NodeItem(const string &key, const string &value);

        ~NodeItem();
    };

    class Node {
    public:
        pgnum pageNum;
        vector<NodeItem> items;
        vector<pgnum> childNodes;
        Dal *dal;
        Tx *tx; // associated transaction

        Node();

        bool isLeaf() const;

        bytearray serialize(bytearray buf, size_t bufSize);

        void deserialize(bytearray buf);

        Node *get(pgnum pageNum) const;

        bool write();

        /**
         * Writes all the provided nodes to the node's dal.
         * `this` excluded.
         * * MUST pass nullptr as last argument
         */
        bool writeNodes(Node *first, ...) const;

        /**
         * Deletes the node (marks its associated page as released in the freelist).
         */
        void deleteNode();

        /* Get and Put Methods */

        /**
         * Finds a key in the node's items (performs binary search, since the keys in each node are sorted).
         * Puts in `idx` the index of the item with `key`, if found. If not found, puts the index where it should
         * be inserted.
         * @return If the key was found.
         */
        bool findKeyInNode(const string &key, int *idx);

        /**
         * Searches for `key` in the tree rooted in `this`, meaning it searches in this node and its child nodes.
         * @param key The key to search for
         * @param idx The index of the item containing `key` in the parent node.
         * @param ancestors An optional list of the ancestors of the searched item.
         * @return The parent node of `key`.
         */
        Node *findKey(const string &key, int *idx, vector<int> *ancestors = nullptr, bool exact = true);

        /**
         * Adds the provided item at index. Does not handle splitting the node if max threshold is reached.
         */
        void addItem(NodeItem &item, int idx);

        /**
         * Returns the size on disk (in bytes) of item at index `idx` in this node.
         * Counts the size of key and value, length of key and value, and the size of the offset to the key and value
         * (written at the start of the page).
         */
        int itemSize(int idx);

        int size();

        bool isOverPopulated();

        bool isUnderPopulated();

        /**
         * Splits `nodeToSplit` in case of it being unbalanced. `this` is the parent of `nodeToSplit`.
         * A new node is created, having half of the split node's values.
         * @param nodeToSplit The node to split (a child of this node).
         * @param nodeToSplitIdx The index of the node to split in this node's children list.
         */
        void split(Node *nodeToSplit, int nodeToSplitIdx);

        /**
         * Returns if an element can be safely removed from this node and it won't be under populated.
         */
        bool canSpareAnElement();

        /* Deletion Methods */

        /**
         * Gets the biggest child of the left subtree.
         * To access the predecessor itself: p->items[p->items.size() - 1]
         */
        Node *getPredecessor(int idx, vector<int> *affectedNodes = nullptr);

        /**
         * Gets the smallest child of the right subtree.
         * To access the successor itself: p->items[0]
         */
        Node *getSuccessor(int idx, vector<int> *affectedNodes = nullptr);

        void borrowFromLeft(int idx);

        void borrowFromRight(int idx);

        /**
         * Removes item at `idx`. Assumes that this node is a leaf node.
         */
        void removeItemFromLeaf(int idx);

        /**
         * Removes item at `idx`. Assumes that this node is not a leaf node.
         * @return Affected nodes - to be used later if the tree needs balancing.
         */
        vector<int> removeItemFromInternal(int idx);

        /**
         * Merges a child node at index `idx` to its left sibling.
         * Deletes the node at `idx`.
         * @return The sibling node, as the child node is deleted.
         */
        Node *merge(int idx);

        /**
         * Rebalances a node after _remove was performed.
         * Decides weather to borrow an element from right or left sibling, or to perform merge.
         * @param unbalancedNodeIdx Unbalanced node index in this node's children list.
         * @return The node at index `unbalancedNodeIdx` or its sibling if it got merged to it (to be replaced in the ancestors list).
         */
        Node *rebalanceAfterRemove(int unbalancedNodeIdx);
    };
}

#endif //NOSQL_DB_NODE_H
