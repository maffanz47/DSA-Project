#ifndef HASH_H
#define HASH_H

#include <string>
#include <vector>

using namespace std;

struct HashNode
{
    string k;
    int v;
    HashNode *nxt;
    HashNode(string key, int val) : k(key), v(val), nxt(nullptr) {}
};

class Hash
{
private:
    int sz;
    vector<HashNode *> b;

    int get_h(string s)
    {
        long long h = 0;
        for (char c : s)
            h = (h * 31 + c) % sz;
        return (int)(h < 0 ? -h : h);
    }

public:
    Hash(int s) : sz(s)
    {
        b.resize(sz, nullptr);
    }

    void add(string k, int v)
    {
        int h = get_h(k);
        HashNode *n = new HashNode(k, v);
        n->nxt = b[h];
        b[h] = n;
    }

    int get(string k)
    {
        int h = get_h(k);
        HashNode *curr = b[h];
        while (curr)
        {
            if (curr->k == k)
                return curr->v;
            curr = curr->nxt;
        }
        return -1;
    }
};

#endif