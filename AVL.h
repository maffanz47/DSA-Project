#ifndef AVL_H
#define AVL_H

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct AVLNode {
    double value;
    vector<int> rowIDs; 
    AVLNode *left;
    AVLNode *right;
    int height;

    AVLNode(double v, int id) : value(v), left(nullptr), right(nullptr), height(1) {
        rowIDs.push_back(id);
    }
};

class AVLTree {
public:
    AVLNode *root;

    AVLTree() : root(nullptr) {}

    int getHeight(AVLNode *n) {
        return n ? n->height : 0;
    }

    int getBalance(AVLNode *n) {
        return n ? getHeight(n->left) - getHeight(n->right) : 0;
    }

    AVLNode *rightRotate(AVLNode *y) {
        AVLNode *x = y->left;
        AVLNode *T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        return x;
    }

    AVLNode *leftRotate(AVLNode *x) {
        AVLNode *y = x->right;
        AVLNode *T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        return y;
    }

    AVLNode *insert(AVLNode *node, double value, int rowID) {
        if (!node) return new AVLNode(value, rowID);

        if (value < node->value)
            node->left = insert(node->left, value, rowID);
        else if (value > node->value)
            node->right = insert(node->right, value, rowID);
        else {
            node->rowIDs.push_back(rowID); 
            return node;
        }

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        int balance = getBalance(node);

        if (balance > 1 && value < node->left->value) return rightRotate(node);
        if (balance < -1 && value > node->right->value) return leftRotate(node);
        if (balance > 1 && value > node->left->value) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        if (balance < -1 && value < node->right->value) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        return node;
    }

    void add(double value, int rowID) {
        root = insert(root, value, rowID);
    }
    void query(AVLNode *node, double minV, double maxV, const vector<vector<string>> &data) {
        if (!node) return;

        if (minV < node->value)
            query(node->left, minV, maxV, data);

        if (node->value >= minV && node->value <= maxV) {
            for (int id : node->rowIDs) {
                cout << "Row " << id << " (" << node->value << "): ";
                for(const auto& cell : data[id]) cout << cell << " | ";
                cout << endl;
            }
        }

        if (maxV > node->value)
            query(node->right, minV, maxV, data);
    }
};

#endif