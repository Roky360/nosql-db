#include "container.h"
#include "../../utils/logging/logging.h"

Container::Container() : root(0), dal(nullptr) {}

NodeItem *Container::get(const string &key) const {
    // get root
    Node *rootNode = this->dal->getNode(this->root);
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

Container *Container::put(const string &key, const string &value) {
    NodeItem item(key, value);

    Node *rootNode;
    // if root does not exist (on first insertion)
    if (this->root == 0) {
        rootNode = new Node();
        rootNode->items.push_back(item);
        bool success = this->dal->writeNode(rootNode);
        if (!success) {
            // TODO: THROW IO ERROR
            cerr << "Uh error " << __FILE_NAME__ << ":" << __LINE__ << endl;
            exit(1);
        }
        this->root = rootNode->pageNum;
        delete rootNode;
        return this;
    } else {
        rootNode = this->dal->getNode(this->root);
        if (rootNode == nullptr) {
            logError("Err");
        }
    }
    rootNode->dal = this->dal;

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

    // get ancestor nodes and rebalance them if needed
    vector<Node *> ancestorsNodes = this->dal->getNodes(rootNode, ancestorsIndexes);
    ancestorsIndexes.insert(ancestorsIndexes.begin(), 0); // insert dummy value for root node

    // go through all ancestors from bottom to top and check if rebalancing is needed.
    for (int i = (int) ancestorsNodes.size() - 2; i >= 0; i--) {
        Node *parentNode = ancestorsNodes[i];
        Node *childNode = ancestorsNodes[i + 1];
        int childIdx = ancestorsIndexes[i + 1];
        if (this->dal->isNodeOverPopulated(childNode)) {
            parentNode->split(childNode, childIdx);
        }
    }

    // balance root if needed
    if (this->dal->isNodeOverPopulated(rootNode)) {
        Node *newRoot = new Node();
        newRoot->dal = this->dal;
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

Container *Container::remove(const string &key) {
    Node *rootNode = this->dal->getNode(this->root);
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

    // get ancestor nodes and rebalance them if needed
    vector<Node *> ancestorsNodes = this->dal->getNodes(rootNode, ancestorsIndexes);
    ancestorsIndexes.insert(ancestorsIndexes.begin(), 0); // insert dummy value for root node

    // balance ancestors if needed
    for (int i = (int) ancestorsNodes.size() - 2; i >= 0; i--) {
        Node *parentNode = ancestorsNodes[i];
        Node *childNode = ancestorsNodes[i + 1];
        int childIdx = ancestorsIndexes[i + 1];
        if (this->dal->isNodeUnderPopulated(childNode)) {
            Node *newChild = parentNode->rebalanceAfterRemove(childIdx);
            if (newChild) { // if the rebalance function returned new child, replace it in the ancestors list
                delete ancestorsNodes[i + 1];
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
