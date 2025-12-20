#ifndef UNIONFIND_H
#define UNIONFIND_H

#include <vector>

using namespace std;

class UnionFind
{
public:
    vector<int> p;

    UnionFind(int n)
    {
        p.resize(n);
        for (int i = 0; i < n; i++)
            p[i] = i;
    }

    int find(int i)
    {
        if (p[i] == i)
            return i;
        return p[i] = find(p[i]);
    }

    void unite(int i, int j)
    {
        int r1 = find(i);
        int r2 = find(j);
        if (r1 != r2)
            p[r1] = r2;
    }
};

#endif