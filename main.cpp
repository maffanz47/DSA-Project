#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip> 
#include "SegmentTree.h"
#include "Trie.h"

using namespace std;

double safeStod(string s, int rowIndex, string colName) {
    try {
        if (s.empty()) return 0.0;
        return stod(s); 
    } catch (...) {
        return 0.0; 
    }
}

bool isNumeric(const string& s) {
    if (s.empty()) return false;
    bool decimalPoint = false;
    for (int i = 0; i < s.length(); i++) {
        if (i == 0 && s[i] == '-') continue;
        if (s[i] == '.') {
            if (decimalPoint) return false;
            decimalPoint = true;
            continue;
        }
        if (!isdigit(s[i])) return false;
    }
    return true;
}

void loadDictionary(Trie &trie, string filename) {
    ifstream file(filename);
    string word;
    if (!file.is_open()) {
        cerr << "Error: Could not find " << filename << endl;
        return;
    }
    int count = 0;
    while (file >> word) {
        trie.insert(word);
        count++;
    }
    file.close();
    cout << "Success: Loaded " << count << " words into the Trie." << endl;
}

int main() {
    Trie dictionary;
    loadDictionary(dictionary, "google-10000-english.txt");

    string filename = "data.csv"; 
    ifstream csvFile(filename);
    if (!csvFile.is_open()) {
        cout << "Could not open " << filename << ". Please ensure it exists." << endl;
        return 1;
    }

    vector<string> headers;
    vector<vector<string>> data;
    string line;

    if (getline(csvFile, line)) {
        stringstream ss(line);
        string cell;
        while (getline(ss, cell, ',')) headers.push_back(cell);
    }

    while (getline(csvFile, line)) {
        vector<string> row;
        bool inQuotes = false;
        string currentCell = "";

        for (char c : line) {
            if (c == '"') {
                inQuotes = !inQuotes; 
            } else if (c == ',' && !inQuotes) {
                row.push_back(currentCell);
                currentCell = "";
            } else {
                currentCell += c;
            }
        }
        row.push_back(currentCell); 
        data.push_back(row);
    }
    csvFile.close();

    cout << "\n--- CSV Column Selection ---" << endl;
    for (int i = 0; i < (int)headers.size(); i++) {
        cout << i << ". " << headers[i] << endl;
    }

    int choice;
    cout << "\nWhich column number would you like to analyze? ";
    cin >> choice;

    if (choice < 0 || choice >= (int)headers.size()) {
        cout << "Invalid selection." << endl;
        return 1;
    }

    if (data.empty()) {
        cout << "The dataset is empty." << endl;
        return 0;
    }

    if (isNumeric(data[0][choice])) {

        cout << "Detected Numeric Column. Building Segment Tree..." << endl;
        vector<double> numbers;
        
        for (int i = 0; i < (int)data.size(); i++) {
            if (choice < (int)data[i].size()) {
                numbers.push_back(safeStod(data[i][choice], i, headers[choice]));
            } else {
                numbers.push_back(0.0);
            }
        }

        SegmentTree st(numbers);
        Node stats = st.getFullStats();
        
        cout << fixed << setprecision(4); 
        cout << "\n--- Statistics for " << headers[choice] << " ---" << endl;
        cout << "Total Sum: " << stats.sum << endl;
        cout << "Minimum:   " << stats.minVal << endl;
        cout << "Maximum:   " << stats.maxVal << endl;

    } else {
        cout << "\n[Notice]: Stats of '" << headers[choice] 
             << "' column can't be found because it contains text/names." << endl;
        cout << "Statistics are only available for numerical columns." << endl;
        
        cout << "\nScanning '" << headers[choice] << "' for inconsistencies using Trie..." << endl;
        
        for (int i = 0; i < (int)data.size(); i++) {
            if (choice >= (int)data[i].size()) continue;
            string currentVal = data[i][choice];
            if (currentVal.empty()) continue;
            
            if (!dictionary.search(currentVal)) {
                cout << "Inconsistency at Row " << i << ": '" << currentVal << "'" << endl;

                string prefix = (currentVal.length() >= 3) ? currentVal.substr(0, 3) : currentVal;
                vector<string> fixes = dictionary.suggest(prefix); 
                if (!fixes.empty()) {
                    cout << "  Suggestions: ";
                    for (int j = 0; j < (int)fixes.size() && j < 3; j++) {
                        cout << fixes[j] << (j == 2 || j == (int)fixes.size()-1 ? "" : ", ");
                    }
                    cout << endl;
                }
            }
        }
    }

    return 0;
}