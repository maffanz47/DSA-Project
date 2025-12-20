#ifndef SEGMENTTREE_H
#define SEGMENTTREE_H

#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

struct Node
{
    double sum;
    double minVal;
    double maxVal;

    Node()
    {
        sum = 0;
        minVal = 1e18;
        maxVal = -1e18;
    }
};

class SegmentTree
{
private:
    int n;
    vector<Node> tree;
    vector<double> originalData;

    Node merge(Node left, Node right)
    {
        Node parent;
        parent.sum = left.sum + right.sum;
        parent.minVal = min(left.minVal, right.minVal);
        parent.maxVal = max(left.maxVal, right.maxVal);
        return parent;
    }

    void build(int node, int start, int end)
    {
        if (start == end)
        {
            tree[node].sum = originalData[start];
            tree[node].minVal = originalData[start];
            tree[node].maxVal = originalData[start];
            return;
        }
        int mid = (start + end) / 2;
        build(2 * node, start, mid);
        build(2 * node + 1, mid + 1, end);
        tree[node] = merge(tree[2 * node], tree[2 * node + 1]);
    }

public:
    SegmentTree(const vector<double> &data)
    {
        originalData = data;
        n = data.size();
        if (n > 0)
        {
            tree.resize(4 * n);
            build(1, 0, n - 1);
        }
    }

    Node getFullStats() { return tree[1]; }
};

#endif