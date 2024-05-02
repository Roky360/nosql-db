#include "containers.h"
#include "../../utils/logging/logging.h"

Container::Container() : root(0), dal(nullptr), tx(nullptr) {}

NodeItem *Container::serialize() const {
    auto *item = new NodeItem();
    item->key = this->id;
    item->value = string();
    item->value.append(to_string(this->root));

    return item;
}

void Container::deserialize(NodeItem *item) {
    this->id = string(item->key);
    this->root = stoi(item->value);
}

NodeItem *Container::_get(const string &key) const {
    // _get root
    Node *rootNode = this->tx->getNode(this->root);
    if (rootNode == nullptr) {
        return nullptr;
    }

    int itemIdx;
    Node *itemNode = rootNode->findKey(key, &itemIdx);
    if (itemNode == nullptr) {
        return nullptr;
    }
    return &itemNode->items[itemIdx];
}

Container *Container::_put(const string &key, const string &value) {
    NodeItem item(key, value);

    Node *rootNode;
    // if root does not exist (on first insertion)
    if (this->root == 0) {
        rootNode = new Node();
        rootNode->items.push_back(item);
        bool success = this->tx->writeNode(rootNode);
        if (!success) {
            logError("IO Error (err writing node)");
        }
        this->root = rootNode->pageNum;
        return this;
    } else {
        rootNode = this->tx->getNode(this->root);
        if (rootNode == nullptr) {
            logError("Err");
        }
    }
    rootNode->dal = this->dal;
    rootNode->tx = this->tx;

    int itemIdx;
    vector<int> ancestorsIndexes;
    Node *itemNode = rootNode->findKey(key, &itemIdx, &ancestorsIndexes, false);

    if (itemNode != nullptr && itemIdx < itemNode->items.size() && itemNode->items[itemIdx].key == key) {
        itemNode->items[itemIdx] = item;
    } else {
        itemNode->addItem(item, itemIdx);
    }
    if (!itemNode->write()) {
        logError("Err");
    }

    // _get ancestor nodes and rebalance them if needed
    vector<Node *> ancestorsNodes = this->_getNodes(ancestorsIndexes);
    ancestorsIndexes.insert(ancestorsIndexes.begin(), 0); // insert dummy value for root node

    // go through all ancestors from bottom to top and check if rebalancing is needed.
    for (int i = (int) ancestorsNodes.size() - 2; i >= 0; i--) {
        Node *parentNode = ancestorsNodes[i];
        Node *childNode = ancestorsNodes[i + 1];
        int childIdx = ancestorsIndexes[i + 1];
        if (childNode->isOverPopulated()) {
            parentNode->split(childNode, childIdx);
        }
    }

    // balance root if needed
    if (rootNode->isOverPopulated()) {
        Node *newRoot = new Node();
        newRoot->dal = this->dal;
        newRoot->tx = this->tx;
        newRoot->childNodes.push_back(rootNode->pageNum);
        newRoot->split(rootNode, 0);
        // write new root
        if (!newRoot->write()) {
            logError("Err");
        }
        // update root
        if (this->dal->meta->root == this->root) {
            this->dal->meta->root = newRoot->pageNum;
        }
        this->root = newRoot->pageNum;
    }

    return this;
}

Container *Container::_remove(const string &key) {
    Node *rootNode = this->tx->getNode(this->root);
    if (rootNode == nullptr) {
        logError("Err");
    }

    int itemIdx;
    vector<int> ancestorsIndexes;
    Node *itemNode = rootNode->findKey(key, &itemIdx, &ancestorsIndexes);
    // if item to delete doesn't exist, do nothing and return
    if (itemNode == nullptr) {
        return this;
    }

    if (itemNode->isLeaf()) {
        itemNode->removeItemFromLeaf(itemIdx);
    } else {
        vector<int> affectedNodes = itemNode->removeItemFromInternal(itemIdx);
        ancestorsIndexes.insert(ancestorsIndexes.end(), affectedNodes.begin(), affectedNodes.end());
    }

    // _get ancestor nodes and rebalance them if needed
    vector<Node *> ancestorsNodes = this->_getNodes(ancestorsIndexes);
    ancestorsIndexes.insert(ancestorsIndexes.begin(), 0); // insert dummy value for root node

    // balance ancestors if needed
    for (int i = (int) ancestorsNodes.size() - 2; i >= 0; i--) {
        Node *parentNode = ancestorsNodes[i];
        Node *childNode = ancestorsNodes[i + 1];
        int childIdx = ancestorsIndexes[i + 1];
        if (childNode->isOverPopulated()) {
            Node *newChild = parentNode->rebalanceAfterRemove(childIdx);
            if (newChild) { // if the rebalance function returned new child, replace it in the ancestors list
//                delete ancestorsNodes[i + 1];
                ancestorsNodes[i + 1] = newChild;
            }
        }
    }

    // balance root if needed
    if (rootNode->items.empty() && !rootNode->childNodes.empty()) {
        rootNode->deleteNode();
        this->root = ancestorsNodes[1]->pageNum;
        // TODO: CHANGE this \/
        this->dal->meta->root = this->root;
    }

    return this;
}

vector<Node *> Container::_getNodes(const vector<int> &indexes) {
    Node *rootNode = this->tx->getNode(this->root);
    if (!rootNode)
        logError("Error reading node");

    vector<Node *> nodes = {rootNode};
    Node *currNode = rootNode;
    for (int i: indexes) {
        currNode = this->tx->getNode(currNode->childNodes[i]);
        if (!currNode) {
            logError("Error reading node");
        }
        nodes.push_back(currNode);
    }

    return nodes;
}
