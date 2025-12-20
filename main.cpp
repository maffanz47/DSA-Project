#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <queue>
#include "SegmentTree.h"
#include "Trie.h"
#include "Hash.h"
#include "UnionFind.h"

using namespace std;

struct RowError
{
    int id;
    int score;
    bool operator<(const RowError &other) const
    {
        return score < other.score;
    }
};

bool is_num(string s)
{
    if (s.empty())
        return false;
    bool dot = false;
    for (int i = 0; i < s.length(); i++)
    {
        if (i == 0 && s[i] == '-')
            continue;
        if (s[i] == '.')
        {
            if (dot)
                return false;
            dot = true;
            continue;
        }
        if (!isdigit(s[i]))
            return false;
    }
    return true;
}

double safe_stod(string s)
{
    try
    {
        if (s.empty() || s == " ")
            return 0.0;
        return stod(s);
    }
    catch (...)
    {
        return 0.0;
    }
}

void load_dict(Trie &t, string fn)
{
    ifstream f(fn);
    string w;
    if (!f.is_open())
    {
        cout << "Dictionary file missing!" << endl;
        return;
    }
    while (f >> w)
        t.insert(w);
    f.close();
}

void display_data(const vector<string> &head, const vector<vector<string>> &data)
{
    cout << "\n--- Dataset Preview ---" << endl;
    for (string h : head)
        cout << left << setw(10) << h;
    cout << endl
         << string(head.size() * 15, '-') << endl;
    for (int i = 0; i < min((int)data.size(), 5); i++)
    {
        for (string cell : data[i])
            cout << left << setw(10) << (cell.length() > 14 ? cell.substr(0, 11) + "..." : cell);
        cout << endl;
    }
}

void remove_column(vector<string> &head, vector<vector<string>> &data)
{
    cout << "Enter column index to remove or -1: ";
    int rem;
    cin >> rem;
    if (rem >= 0 && rem < (int)head.size())
    {
        head.erase(head.begin() + rem);
        for (auto &r : data)
            if (rem < (int)r.size())
                r.erase(r.begin() + rem);
        cout << "Column removed successfully." << endl;
    }
}

void handle_duplicates(vector<vector<string>> &data)
{
    cout << "Scanning for duplicates..." << endl;
    Hash h_map(data.size() * 2);
    UnionFind dsu(data.size());
    int d_cnt = 0;

    for (int i = 0; i < (int)data.size(); i++)
    {
        string key = "";
        for (string s : data[i])
            key += s + "|";
        int match = h_map.get(key);
        if (match != -1)
        {
            dsu.unite(i, match);
            d_cnt++;
        }
        else
        {
            h_map.add(key, i);
        }
    }

    cout << "Duplicates found: " << d_cnt << ". Merge unique rows? (1:Yes, 0:No): ";
    int choice;
    cin >> choice;
    if (choice == 1)
    {
        vector<vector<string>> clean;
        vector<bool> seen(data.size(), false);
        for (int i = 0; i < (int)data.size(); i++)
        {
            int root = dsu.find(i);
            if (!seen[root])
            {
                clean.push_back(data[i]);
                seen[root] = true;
            }
        }
        data = clean;
        cout << "Duplicates removed. New row count: " << data.size() << endl;
    }
}

void impute_missing(const vector<string> &head, vector<vector<string>> &data)
{
    cout << "Imputing missing numeric values..." << endl;
    for (int j = 0; j < (int)head.size(); j++)
    {
        double sum = 0;
        int cnt = 0;
        for (int i = 0; i < (int)data.size(); i++)
        {
            if (is_num(data[i][j]))
            {
                sum += safe_stod(data[i][j]);
                cnt++;
            }
        }

        if (cnt > 0)
        {
            string avg = to_string(sum / cnt);
            for (int i = 0; i < (int)data.size(); i++)
                if (data[i][j] == "" || data[i][j] == " ")
                    data[i][j] = avg;
        }
    }
    cout << "Done." << endl;
}

void show_priority_rows(const vector<string> &head, const vector<vector<string>> &data, Trie &dict)
{
    priority_queue<RowError> pq;
    for (int i = 0; i < (int)data.size(); i++)
    {
        int score = 0;
        for (int j = 0; j < (int)head.size(); j++)
        {
            if (data[i][j].empty() || data[i][j] == " ")
                score += 2;
            else if (!is_num(data[i][j]) && !dict.search(data[i][j]))
                score += 1;
        }
        if (score > 0)
            pq.push({i, score});
    }

    cout << "\n--- Top 5 Rows Needing Attention ---" << endl;
    for (int i = 0; i < 5 && !pq.empty(); i++)
    {
        RowError top = pq.top();
        pq.pop();
        cout << "Row " << top.id << " | Dirty Score: " << top.score << endl;
    }
}

void analyze_column(const vector<string> &head, const vector<vector<string>> &data, Trie &dict)
{
    cout << "Select Column (0-" << head.size() - 1 << "): ";
    int sel;
    cin >> sel;
    if (sel < 0 || sel >= (int)head.size())
        return;

    if (is_num(data[0][sel]))
    {

        vector<double> nums;
        for (auto &r : data)
            nums.push_back(safe_stod(r[sel]));
        SegmentTree st(nums);
        Node res = st.getFullStats();
        cout << "Sum: " << res.sum << " | Min: " << res.minVal << " | Max: " << res.maxVal << endl;
    }
    else
    {

        cout << "Checking for typos..." << endl;
        for (int i = 0; i < (int)data.size(); i++)
        {
            if (!data[i][sel].empty() && !dict.search(data[i][sel]))
            {
                cout << "Row " << i << ": " << data[i][sel];
                vector<string> sug = dict.suggest(data[i][sel].substr(0, 3));
                if (!sug.empty())
                    cout << " -> Try: " << sug[0];
                cout << endl;
            }
        }
    }
}

int main()
{
    Trie dict;
    load_dict(dict, "google-10000-english.txt");

    string fn = "data.csv";
    ifstream file(fn);
    if (!file.is_open())
    {
        cout << "Could not open " << fn << endl;
        return 1;
    }

    vector<string> head;
    vector<vector<string>> data;
    string line;

    if (getline(file, line))
    {
        stringstream ss(line);
        string c;
        while (getline(ss, c, ','))
            head.push_back(c);
    }

    while (getline(file, line))
    {
        vector<string> r;
        bool in_q = false;
        string cur = "";
        for (char ch : line)
        {
            if (ch == '"')
                in_q = !in_q;
            else if (ch == ',' && !in_q)
            {
                r.push_back(cur);
                cur = "";
            }
            else
                cur += ch;
        }
        r.push_back(cur);
        data.push_back(r);
    }
    file.close();

    int choice = 0;
    while (choice != 7)
    {
        cout << "\n--- Smart Data Cleaning Engine ---" << endl;
        cout << "1. Display Current Data" << endl;
        cout << "2. Remove Columns" << endl;
        cout << "3. Handle Duplicates" << endl;
        cout << "4. Fill Missing Values" << endl;
        cout << "5. Prioritize Cleaning" << endl;
        cout << "6. Analyze Column" << endl;
        cout << "7. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            display_data(head, data);
            break;
        case 2:
            remove_column(head, data);
            break;
        case 3:
            handle_duplicates(data);
            break;
        case 4:
            impute_missing(head, data);
            break;
        case 5:
            show_priority_rows(head, data, dict);
            break;
        case 6:
            analyze_column(head, data, dict);
            break;
        case 7:
            cout << "Exiting..." << endl;
            break;
        default:
            cout << "Invalid choice!" << endl;
        }
    }
    return 0;
}