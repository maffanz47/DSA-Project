#ifndef TRIE_H
#define TRIE_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct TrieNode
{
    TrieNode *children[26];
    bool isEndOfWord;
    TrieNode()
    {
        isEndOfWord = false;
        for (int i = 0; i < 26; i++)
        {
            children[i] = nullptr;
        }
    }
};

class Trie
{
private:
    TrieNode *root;

    void deleteTrie(TrieNode *node)
    {
        if (!node)
            return;
        for (int i = 0; i < 26; i++)
        {
            deleteTrie(node->children[i]);
        }
        delete node;
    }

    void findWords(TrieNode *node, string currentPrefix, vector<string> &results)
    {
        if (node->isEndOfWord)
        {
            results.push_back(currentPrefix);
        }

        for (int i = 0; i < 26; i++)
        {
            if (node->children[i])
            {
                char nextChar = 'a' + i;
                findWords(node->children[i], currentPrefix + nextChar, results);
            }
        }
    }

    int getIndex(char c)
    {
        if (c >= 'A' && c <= 'Z')
            return c - 'A';
        if (c >= 'a' && c <= 'z')
            return c - 'a';
        return -1;
    }

public:
    Trie()
    {
        root = new TrieNode();
    }

    ~Trie()
    {
        deleteTrie(root);
    }

    void insert(string word)
    {
        TrieNode *crawler = root;
        for (char c : word)
        {
            int index = getIndex(c);
            if (index == -1)
                continue;

            if (!crawler->children[index])
            {
                crawler->children[index] = new TrieNode();
            }
            crawler = crawler->children[index];
        }
        crawler->isEndOfWord = true;
    }

    bool search(string word)
    {
        TrieNode *crawler = root;
        for (char c : word)
        {
            int index = getIndex(c);
            if (index == -1)
                return false;

            if (!crawler->children[index])
            {
                return false;
            }
            crawler = crawler->children[index];
        }
        return crawler->isEndOfWord;
    }

    vector<string> suggest(string prefix)
    {
        vector<string> suggestions;
        TrieNode *crawler = root;
        for (char c : prefix)
        {
            int index = getIndex(c);
            if (index == -1)
                continue;

            if (!crawler->children[index])
            {
                return suggestions;
            }
            crawler = crawler->children[index];
        }

        findWords(crawler, prefix, suggestions);
        return suggestions;
    }
};

#endif