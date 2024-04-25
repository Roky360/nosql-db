#include "document.h"

Document::Document() : root(0), dal(nullptr) {}

NodeItem *Document::serialize() const {
    auto *item = new NodeItem();
    item->key = this->id;
    item->value = string();
    item->value.append(to_string(this->root));

    return item;
}

void Document::deserialize(NodeItem *item) {
    this->id = string(item->key);
    this->root = stoi(item->value);
}

NodeItem *Document::get(const string &key) const {
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

Document *Document::put(const string &key, const string &value) {
    NodeItem item(key, value);

    Node *rootNode;
    // if root does not exist (on first insertion)
    if (this->root == 0) {
        rootNode = new Node();
        rootNode->items.push_back(item);
        bool success = this->dal->writeNode(rootNode);
        if (!success) {
            // TODO: THROW IO ERROR
            cerr << "Uh error" << endl;
            exit(1);
        }
        this->root = rootNode->pageNum;
        delete rootNode;
        return this;
    } else {
        rootNode = this->dal->getNode(this->root);
        if (rootNode == nullptr) {
            // TODO: THROW IO ERROR
            cerr << "Uh error" << endl;
            exit(1);
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
        // TODO: THROW IO ERROR
        cerr << "Uh error" << endl;
        exit(1);
    }

    // get ancestor nodes and rebalance them if needed
    vector<Node *> ancestorsNodes;
    ancestorsNodes.push_back(rootNode);
    Node *currNode = rootNode;
    for (int i : ancestorsIndexes) {
        currNode = this->dal->getNode(currNode->childNodes[i]);
        if (!currNode) {
            // TODO: THROW IO ERROR
            cerr << "Uh error" << endl;
            exit(1);
        }
        ancestorsNodes.push_back(currNode);
    }
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

    // check if root needs rebalancing
    if (this->dal->isNodeOverPopulated(rootNode)) {
        Node *newRoot = new Node();
        newRoot->dal = this->dal;
        newRoot->childNodes.push_back(rootNode->pageNum);
        newRoot->split(rootNode, 0);

        // write new root
        if (!newRoot->write()) {
            // TODO: THROW IO ERROR
            cerr << "Uh error" << endl;
            exit(1);
        }
        // update root
        if (this->dal->meta->root == this->root) {
            this->dal->meta->root = newRoot->pageNum;
        }
        this->root = newRoot->pageNum;
    }

    return this;
}
