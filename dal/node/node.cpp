#include <string>
#include <cstdarg>
#include "node.h"
#include "../../utils/ioutils.h"

using namespace ioutils;

namespace dal {
    NodeItem::NodeItem() = default;

    NodeItem::NodeItem(const string &key, const string &value) : key(string(key)), value(string(value)) {}

    NodeItem::~NodeItem() = default;

    /* Node */

    Node::Node() : pageNum(0), dal(nullptr) {}

    bool Node::isLeaf() const {
        return this->childNodes.empty();
    }

    bytearray Node::serialize(bytearray buf, size_t bufSize) {
        size_t startPos = 0;
        size_t endPos = bufSize - 1;
        /* write header */
        // write is leaf
        writeIntToStream(buf, startPos, (char) isLeaf());
        // write number of key-value pairs
        writeIntToStream(buf, startPos, (__int16) this->items.size());

        // write key-value pairs, appended to the end of the buffer
        int i;
        for (i = 0; i < this->items.size(); i++) {
            NodeItem item = this->items[i];

            // write child nodes' page numbers
            if (!isLeaf()) {
                writeIntToStream(buf, startPos, this->childNodes[i]);
            }

            char kLen = item.key.length();
            char vLen = item.value.length();

            __int16 offset = endPos - kLen - vLen - sizeof(kLen) - sizeof(vLen);
            writeIntToStream(buf, startPos, offset);

            // write value (size of value and value itself)
            endPos -= vLen;
            item.value.copy(buf + endPos, vLen);
            endPos -= sizeof(vLen);
            writeIntToStream(buf, endPos, vLen, false);
            // write key
            endPos -= kLen;
            item.key.copy(buf + endPos, kLen);
            endPos -= sizeof(vLen);
            writeIntToStream(buf, endPos, kLen, false);
        }

        // write the last child's page numbers
        if (!isLeaf()) {
            writeIntToStream(buf, startPos, this->childNodes[i]);
        }

        return buf;
    }

    void Node::deserialize(bytearray buf) {
        size_t startPos = 0;

        bool isLeaf = buf[startPos++];
        __int16 itemCount = readIntFromStream<__int16>(buf, startPos);

        // read items
        int i;
        for (i = 0; i < itemCount; i++) {
            NodeItem item;
            if (!isLeaf) {
                this->childNodes.push_back(readIntFromStream(buf, startPos));
            }

            // get offset of the current key-value pair
            size_t offset = readIntFromStream<__int16>(buf, startPos);
            // read key
            char kLen = readIntFromStream<char>(buf, offset);
            item.key = string(buf + offset, kLen);
            offset += kLen;
            // read value
            char vLen = readIntFromStream<char>(buf, offset);
            item.value = string(buf + offset, vLen);
            offset += vLen;

            this->items.push_back(item);
        }

        if (!isLeaf) {
            this->childNodes.push_back(readIntFromStream(buf, startPos));
        }
    }

    Node *Node::get(pgnum pageNum) const {
        return this->dal->getNode(pageNum);
    }

    bool Node::write() {
        return this->dal->writeNode(this);
    }

    bool Node::writeNodes(Node *first, ...) const {
        va_list args;
        va_start(args, first);
        bool res = this->dal->__writeNodes(first, args);
        va_end(args);
        return res;
    }

    void Node::deleteNode() {
        this->dal->deleteNode(this);
    }

    bool Node::findKeyInNode(const string &key, int *idx) {
        int leftPos = 0, rightPos = (int) this->items.size() - 1;
        int mid = 0, cmpRes = 1;

        while (leftPos <= rightPos) {
            mid = (leftPos + rightPos) / 2;
            cmpRes = this->items[mid].key.compare(key);

            // item found
            if (cmpRes == 0) {
                *idx = mid;
                return true;
            }
            // key is in left half (ordered before the current item)
            if (cmpRes > 0) {
                rightPos = mid - 1;
            } else {
                leftPos = mid + 1;
            }
        }

        // item not found, return where it should be inserted, and `false` for not found
        if (cmpRes > 0) {
            *idx = max(0, mid - 1);
        } else {
            *idx = mid + 1;
        }
        return false;
    }

    Node *Node::findKey(const string &key, int *idx, vector<int> *ancestorsIndexes, bool exact) {
        if (findKeyInNode(key, idx)) {
            return this;
        }
        if (isLeaf()) {
            if (!exact) // idx is not set to -1, instead it is set to the index the item should be inserted
                return this;
            *idx = -1;
            return nullptr;
        }

        if (ancestorsIndexes)
            ancestorsIndexes->push_back(*idx);
        Node *child = this->dal->getNode(this->childNodes[*idx]);
        if (!child) {
            *idx = -1;
            return nullptr;
        }
        return child->findKey(key, idx, ancestorsIndexes, exact);
    }

    void Node::addItem(NodeItem &item, int idx) {
        if (idx < 0 || idx > this->items.size())
            return;

        if (idx == this->items.size()) {
            this->items.push_back(item);
        } else {
            this->items.insert(this->items.begin() + idx, item);
        }
    }

    int Node::itemSize(int idx) {
        int size = 0;
        NodeItem &item = this->items[idx];
        size += sizeof(__int16); // size of offset
        size += sizeof(char) * 2; // size of length of the key and value
        size += item.key.length();
        size += item.value.length();
        size += isLeaf() ? 0
                         : sizeof(this->childNodes[idx]); // add size of reference to child node, if this node is not a leaf
        return size;
    }

    int Node::size() {
        int size = 0;
        size += NODE_HEADER_SIZE;
        // add size of items
        for (int i = 0; i < this->items.size(); i++) {
            size += itemSize(i);
        }
        // if not leaf, size of reference to last child (other children were added in itemSize method)
        if (!isLeaf()) {
            size += sizeof(pgnum);
        }

        return size;
    }

    void Node::split(Node *nodeToSplit, int nodeToSplitIdx) {
        int splitIdx = this->dal->getSplitIndex(nodeToSplit);
        if (splitIdx == -1) {
            cerr << "Cannot split node " << nodeToSplit->pageNum << endl;
            exit(1);
        }

        NodeItem &middleItem = nodeToSplit->items[splitIdx];
        Node newNode;
        newNode.dal = this->dal;

        // create the new node and write it
        if (nodeToSplit->isLeaf()) {
            newNode.items.assign(nodeToSplit->items.begin() + splitIdx + 1, nodeToSplit->items.end());
            nodeToSplit->items.erase(nodeToSplit->items.begin() + splitIdx, nodeToSplit->items.end());
        } else {
            // move all items and children before the split index to the new node
            newNode.items.assign(nodeToSplit->items.begin() + splitIdx + 1, nodeToSplit->items.end());
            newNode.childNodes.assign(nodeToSplit->childNodes.begin() + splitIdx + 1, nodeToSplit->childNodes.end());
            // remove those from the node to split
            nodeToSplit->items.erase(nodeToSplit->items.begin() + splitIdx, nodeToSplit->items.end());
            nodeToSplit->childNodes.erase(nodeToSplit->childNodes.begin() + splitIdx, nodeToSplit->childNodes.end());
        }
        newNode.write();

        // add item at split index to parent node
        this->addItem(middleItem, nodeToSplitIdx);
        // add new node as child of the parent node
        this->childNodes.insert(this->childNodes.begin() + nodeToSplitIdx + 1, newNode.pageNum);

        writeNodes(this, nodeToSplit, nullptr);
    }

    bool Node::canSpareAnElement() {
        return this->dal->getSplitIndex(this) != -1;
    }

    /* Deletion */

    Node *Node::getPredecessor(int idx, vector<int> *affectedNodes) {
        Node *cur = this->dal->getNode(this->childNodes[idx]);
        if (!cur) {
            return nullptr;
        }

        while (!cur->isLeaf()) {
            int i = cur->childNodes.size() - 1;
            cur = this->dal->getNode(cur->childNodes[i]);
            if (!cur)
                return nullptr;
            if (affectedNodes)
                affectedNodes->push_back(i);
        }
        return cur;
    }

    Node *Node::getSuccessor(int idx, vector<int> *affectedNodes) {
        Node *cur = this->dal->getNode(this->childNodes[idx + 1]);
        if (!cur) {
            return nullptr;
        }

        while (!cur->isLeaf()) {
            int i = 0;
            cur = this->dal->getNode(cur->childNodes[i]);
            if (!cur)
                return nullptr;
            if (affectedNodes)
                affectedNodes->push_back(i);
        }
        return cur;
    }

    void Node::borrowFromLeft(int idx) {
        Node *child = this->dal->getNode(this->childNodes[idx]);
        Node *sibling = this->dal->getNode(this->childNodes[idx - 1]);
        if (!child || !sibling)
            throw runtime_error("Can't read nodes");

        child->items.insert(child->items.begin(), NodeItem());
        child->items[0] = this->items[idx - 1];

        // move the sibling's last child we are about to move to the parent, to child
        if (!child->isLeaf()) {
            child->childNodes.insert(child->childNodes.begin(),
                                     sibling->childNodes[sibling->childNodes.size() - 1]);
        }

        this->items[idx - 1] = sibling->items[sibling->items.size() - 1];

        writeNodes(this, sibling, child, nullptr);
        delete child;
        delete sibling;
    }

    void Node::borrowFromRight(int idx) {
        Node *child = this->dal->getNode(this->childNodes[idx]);
        Node *sibling = this->dal->getNode(this->childNodes[idx + 1]);
        if (!child || !sibling)
            throw runtime_error("Can't read nodes");

        child->items.push_back(this->items[idx]);

        // move the sibling's last child we are about to move to the parent, to child
        if (!child->isLeaf()) {
            child->childNodes.push_back(sibling->childNodes[0]);
        }

        this->items[idx] = sibling->items[0];

        writeNodes(this, sibling, child, nullptr);
        delete child;
        delete sibling;
    }

    void Node::removeItemFromLeaf(int idx) {
        this->items.erase(this->items.begin() + idx);
        this->write();
    }

    vector<int> Node::removeItemFromInternal(int idx) {
        vector<int> affectedNodes;
        Node *predecessor = getPredecessor(idx, &affectedNodes);
        this->items[idx] = predecessor->items[predecessor->items.size() - 1];
        predecessor->items.pop_back();

        writeNodes(this, predecessor, nullptr);
        return affectedNodes;
    }

    void Node::merge(int idx) {
        Node *child = this->dal->getNode(this->childNodes[idx]);
        Node *sibling = this->dal->getNode(this->childNodes[idx - 1]);
        if (!child || !sibling)
            throw runtime_error("Can't read nodes");

        // take item from the parent, put in sibling
        sibling->items.push_back(this->items[idx - 1]);
        // move all items from child to sibling
        sibling->items.insert(sibling->items.end(), child->items.begin(), child->items.end());
        // if child is not a leaf, move also its children to the sibling
        if (!child->isLeaf()) {
            sibling->childNodes.insert(sibling->childNodes.end(), child->childNodes.begin(), child->childNodes.end());
        }

        // remove the item we took from parent, and the reference to child
        this->childNodes.erase(this->childNodes.begin() + idx);
        this->items.erase(this->items.begin() + (idx - 1));

        child->deleteNode();
        writeNodes(this, sibling, nullptr);
    }

    void Node::rebalanceAfterRemove(int unbalancedNodeIdx) {
        // borrow from left
        if (unbalancedNodeIdx != 0) {
            Node *leftSibling = this->dal->getNode(this->childNodes[unbalancedNodeIdx - 1]);
            if (!leftSibling)
                // TODO: IO ERROR
                exit(1);
            if (leftSibling->canSpareAnElement()) {
                borrowFromLeft(unbalancedNodeIdx);
                delete leftSibling;
                return;
            }
            delete leftSibling;
        }

        // borrow from right
        if (unbalancedNodeIdx != this->childNodes.size() - 1) {
            Node *rightSibling = this->dal->getNode(this->childNodes[unbalancedNodeIdx + 1]);
            if (!rightSibling)
                // TODO: IO ERROR
                exit(1);
            if (rightSibling->canSpareAnElement()) {
                borrowFromRight(unbalancedNodeIdx);
                delete rightSibling;
                return;
            }
            delete rightSibling;
        }

        /* The merge function merges with left sibling, so if the child doesn't have left sibling (its index is 0),
         * then merge the sibling with the child (the order of the parameters change)
         * */
        if (unbalancedNodeIdx == 0) {
            return this->merge(unbalancedNodeIdx + 1);
        } else {
            return this->merge(unbalancedNodeIdx);
        }
    }
}
