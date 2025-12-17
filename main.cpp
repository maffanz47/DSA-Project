#include <iostream>
#include <fstream>
#include <string>
#include "Trie.h"

using namespace std;

void loadDictionary(Trie &trie, string filename)
{
    ifstream file(filename);
    string word;

    if (!file.is_open())
    {
        cerr << "Error: Could not find " << filename << ". Make sure it's in the project folder." << endl;
        return;
    }

    int count = 0;
    while (file >> word)
    { // Reads word by word
        trie.insert(word);
        count++;
    }

    file.close();
    cout << "Success: Loaded " << count << " words into the Trie." << endl;
}

int main()
{
    Trie dictionary;

    loadDictionary(dictionary, "google-10000-english.txt");

    string messyInput = "goverment";

    if (dictionary.search(messyInput))
    {
        cout << "Entry is valid." << endl;
    }
    else
    {
        cout << "'" << messyInput << "' is inconsistent." << endl;

        vector<string> fixes = dictionary.suggest("govern");
        if (!fixes.empty())
        {
            cout << "Found " << fixes.size() << " suggestions:" << endl;
            cout << "-----------------------------------" << endl;
            for (int i = 0; i < fixes.size(); i++)
            {
                // Displaying a numbered list of all matches
                cout << i + 1 << ". " << fixes[i] << endl;
            }
        }
    }

    return 0;
}