#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <numeric> 
#include <algorithm>
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

void displayHead(const vector<string>& headers, const vector<vector<string>>& data) {
    cout << "\n--- Dataset Head (First 5 Rows) ---" << endl;
    for (const string& h : headers) cout << left << setw(15) << (h.length() > 14 ? h.substr(0, 11) + "..." : h);
    cout << endl << string(headers.size() * 15, '-') << endl;

    int rowsToShow = min((int)data.size(), 5);
    for (int i = 0; i < rowsToShow; i++) {
        for (const string& cell : data[i]) {
            cout << left << setw(15) << (cell.length() > 14 ? cell.substr(0, 11) + "..." : cell);
        }
        cout << endl;
    }
}

void imputeMissingWithAverage(vector<vector<string>>& data, int colIndex) {
    double sum = 0;
    int count = 0;
    vector<int> missingIndices;

    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i][colIndex].empty() || data[i][colIndex] == " " ) {
            missingIndices.push_back(i);
        } else {
            try {
                sum += stod(data[i][colIndex]);
                count++;
            } catch (...) {}
        }
    }

    if (count > 0) {
        double avg = sum / count;
        for (int idx : missingIndices) {
            data[idx][colIndex] = to_string(avg);
        }
        cout << "Column '" << colIndex << "': Fixed " << missingIndices.size() << " empty cells with average: " << fixed << setprecision(2) << avg << endl;
    }
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
            if (c == '"') inQuotes = !inQuotes; 
            else if (c == ',' && !inQuotes) {
                row.push_back(currentCell);
                currentCell = "";
            } else currentCell += c;
        }
        row.push_back(currentCell); 
        data.push_back(row);
    }
    csvFile.close();

    displayHead(headers, data);

    cout << "\nStep 1: Column Removal. Enter column index to remove, or -1 to stop: ";
    int toRemove;
    while (cin >> toRemove && toRemove != -1) {
        if (toRemove >= 0 && toRemove < (int)headers.size()) {
            cout << "Removing column: " << headers[toRemove] << endl;
            headers.erase(headers.begin() + toRemove);
            for (auto& row : data) {
                if(toRemove < row.size()) row.erase(row.begin() + toRemove);
            }
            for (int i = 0; i < headers.size(); i++) cout << i << ". " << headers[i] << " | ";
            cout << "\nEnter next index or -1: ";
        }
    }

    cout << "\nStep 2: Removing rows with missing critical data (Column 0)..." << endl;
    int initialSize = data.size();
    data.erase(remove_if(data.begin(), data.end(), [](const vector<string>& row) {
        return row.empty() || row[0].empty() || row[0] == " ";
    }), data.end());
    cout << "Removed " << initialSize - data.size() << " rows." << endl;

    cout << "\nStep 3: Fill missing numerical data with averages? (1=Yes, 0=No): ";
    int choiceAvg;
    cin >> choiceAvg;
    if (choiceAvg == 1) {
        for (int i = 0; i < (int)headers.size(); i++) {
         
            bool foundNumeric = false;
            for(auto& r : data) {
                if(!r[i].empty() && isNumeric(r[i])) {
                    foundNumeric = true;
                    break;
                }
            }
            if (foundNumeric) imputeMissingWithAverage(data, i);
        }
    }

    cout << "\n--- Final CSV Column Selection for Analysis ---" << endl;
    for (int i = 0; i < (int)headers.size(); i++) cout << i << ". " << headers[i] << endl;

    int choice;
    cout << "\nWhich column would you like to analyze with DSA (Segment Tree/Trie)? ";
    cin >> choice;

    if (choice >= 0 && choice < (int)headers.size()) {
        if (isNumeric(data[0][choice])) {
            cout << "Building Segment Tree for stats..." << endl;
            vector<double> numbers;
            for (auto& row : data) numbers.push_back(safeStod(row[choice], 0, headers[choice]));

            SegmentTree st(numbers);
            Node stats = st.getFullStats();
            cout << fixed << setprecision(4) << "\n--- Statistics for " << headers[choice] << " ---" << endl;
            cout << "Total Sum: " << stats.sum << "\nMinimum: " << stats.minVal << "\nMaximum: " << stats.maxVal << endl;
        } else {
            cout << "\n[Notice]: Stats of '" << headers[choice] << "' can't be found (Text Column)." << endl;
            cout << "Scanning for inconsistencies..." << endl;
            for (int i = 0; i < (int)data.size(); i++) {
                string val = data[i][choice];
                if (!val.empty() && !dictionary.search(val)) {
                    cout << "Row " << i << " Inconsistency: '" << val << "'";
                    vector<string> fixes = dictionary.suggest(val.substr(0, 3));
                    if(!fixes.empty()) {
                        cout << " | Suggestions: ";
                        for(int j=0; j<min((int)fixes.size(), 3); j++) cout << fixes[j] << " ";
                    }
                    cout << endl;
                }
            }
        }
    }

    return 0;
}