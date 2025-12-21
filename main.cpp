#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <queue>
#include <math.h>
#include <ctime>
#include "SegmentTree.h"
#include "Trie.h"
#include "Hash.h"
#include "UnionFind.h"
#include "AVL.h"

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

struct Point
{
    double x, y;
    int cluster;
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

class Analytics
{
public:
    static double calculate_correlation(const vector<double> &x, const vector<double> &y)
    {
        if (x.size() != y.size() || x.empty())
            return 0.0;
        double n = x.size();
        double sum_x = accumulate(x.begin(), x.end(), 0.0);
        double sum_y = accumulate(y.begin(), y.end(), 0.0);
        double sum_xy = inner_product(x.begin(), x.end(), y.begin(), 0.0);
        double sum_x2 = inner_product(x.begin(), x.end(), x.begin(), 0.0);
        double sum_y2 = inner_product(y.begin(), y.end(), y.begin(), 0.0);

        double num = n * sum_xy - (sum_x * sum_y);
        double den = sqrt((n * sum_x2 - pow(sum_x, 2)) * (n * sum_y2 - pow(sum_y, 2)));
        return (den == 0) ? 0 : num / den;
    }

    static void run_kmeans(const vector<double> &x, const vector<double> &y, int k)
    {
        int n = x.size();
        vector<Point> points(n);
        for (int i = 0; i < n; i++)
            points[i] = {x[i], y[i], -1};

        vector<Point> centroids(k);
        for (int i = 0; i < k; i++)
            centroids[i] = points[rand() % n];

        for (int iter = 0; iter < 50; iter++)
        {
            for (auto &p : points)
            {
                double min_d = 1e18;
                for (int c = 0; c < k; c++)
                {
                    double d = pow(p.x - centroids[c].x, 2) + pow(p.y - centroids[c].y, 2);
                    if (d < min_d)
                    {
                        min_d = d;
                        p.cluster = c;
                    }
                }
            }
            vector<double> sx(k, 0), sy(k, 0);
            vector<int> counts(k, 0);
            for (const auto &p : points)
            {
                sx[p.cluster] += p.x;
                sy[p.cluster] += p.y;
                counts[p.cluster]++;
            }
            for (int i = 0; i < k; i++)
                if (counts[i] > 0)
                {
                    centroids[i].x = sx[i] / counts[i];
                    centroids[i].y = sy[i] / counts[i];
                }
        }
        visualize(points);
    }

    static void visualize(const vector<Point> &points)
    {
        const int W = 50, H = 15;
        vector<vector<char>> grid(H, vector<char>(W, '.'));
        double min_x = 1e18, max_x = -1e18, min_y = 1e18, max_y = -1e18;
        for (auto &p : points)
        {
            min_x = min(min_x, p.x);
            max_x = max(max_x, p.x);
            min_y = min(min_y, p.y);
            max_y = max(max_y, p.y);
        }
        for (auto &p : points)
        {
            int ix = (int)((p.x - min_x) / (max_x - min_x + 1e-9) * (W - 1));
            int iy = (int)((p.y - min_y) / (max_y - min_y + 1e-9) * (H - 1));
            grid[H - 1 - iy][ix] = '0' + p.cluster;
        }
        for (auto &row : grid)
        {
            for (char c : row)
                cout << c;
            cout << endl;
        }
    }

    static pair<double, double> run_regression(const vector<double> &x, const vector<double> &y, double split_ratio)
    {
        int n = x.size();
        if (n == 0)
            return {0, 0};

        int train_size = (int)(n * split_ratio);

        double sx = 0, sy = 0, sxx = 0, sxy = 0;
        for (int i = 0; i < train_size; i++)
        {
            sx += x[i];
            sy += y[i];
            sxx += x[i] * x[i];
            sxy += x[i] * y[i];
        }

        double denominator = (train_size * sxx - sx * sx);
        if (denominator == 0)
        {
            cout << "Error: All X values are identical. Cannot fit line.\n";
            return {0, 0};
        }

        double slope = (train_size * sxy - sx * sy) / denominator;
        double intercept = (sy - slope * sx) / train_size;

        cout << "\n--- Model Trained on " << train_size << " samples ---\n";
        cout << "Equation: y = " << slope << "x + " << intercept << endl;

        if (n - train_size > 0)
        {
            double total_error = 0;
            cout << "\n--- Testing on " << n - train_size << " samples ---\n";
            for (int i = train_size; i < n; i++)
            {
                double pred = slope * x[i] + intercept;
                double error = abs(pred - y[i]);
                total_error += error;
                if (i < train_size + 5)
                    cout << "Act: " << y[i] << " | Pred: " << pred << " | Diff: " << error << endl;
            }
            cout << "Mean Absolute Error (MAE): " << total_error / (n - train_size) << endl;
        }

        return {slope, intercept};
    }
};

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

void remove_row(vector<vector<string>> &data)
{
    cout << "Enter Row ID to remove (0 to " << data.size() - 1 << "): ";
    int idx;
    cin >> idx;

    if (idx >= 0 && idx < (int)data.size())
    {
        data.erase(data.begin() + idx);
        cout << "Row " << idx << " deleted." << endl;
    }
    else
    {
        cout << "Invalid Row ID." << endl;
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

void show_priority_rows(const vector<string> &head, vector<vector<string>> &data, Trie &dict)
{
<<<<<<< HEAD
    vector<pair<int, int>> row_scores; 
=======

    vector<pair<int, int>> row_scores;

>>>>>>> ee7beaa18d8be7964c278616bc9772ccb059582a
    for (int i = 0; i < (int)data.size(); i++)
    {
        int score = 0;
        for (int j = 0; j < (int)head.size(); j++)
        {
<<<<<<< HEAD
            if (data[i][j].empty() || data[i][j] == " ")
                score += 2;
=======

            if (data[i][j].empty() || data[i][j] == " ")
                score += 2;

>>>>>>> ee7beaa18d8be7964c278616bc9772ccb059582a
            else if (!is_num(data[i][j]) && !dict.search(data[i][j]))
                score += 1;
        }
        row_scores.push_back({score, i});
    }

    sort(row_scores.rbegin(), row_scores.rend());
<<<<<<< HEAD

=======
>>>>>>> ee7beaa18d8be7964c278616bc9772ccb059582a
    cout << "\n--- Top 5 Rows Needing Attention ---" << endl;
    for (int i = 0; i < min(5, (int)row_scores.size()); i++)
    {
        if (row_scores[i].first > 0)
        {
            cout << "Original Row " << row_scores[i].second << " | Dirty Score: " << row_scores[i].first << endl;
        }
    }
    cout << "\nWould you like to sort the dataset to bring these errors to the top? (1:Yes, 0:No): ";
    int choice;
    cin >> choice;

    if (choice == 1)
    {
        vector<vector<string>> sorted_data;
        for (auto &p : row_scores)
        {
            sorted_data.push_back(data[p.second]);
        }
        data = sorted_data; 
        cout << "Dataset sorted! The dirtiest rows are now at the top." << endl;
    }
}
void save_data(const vector<string> &head, const vector<vector<string>> &data, string filename)
{
    ofstream file(filename);
    if (!file.is_open())
    {
        cout << "Error: Could not write to file!" << endl;
        return;
    }
    for (int i = 0; i < head.size(); i++)
    {
        file << head[i] << (i == head.size() - 1 ? "" : ",");
    }
    file << "\n";

    for (const auto &row : data)
    {
        for (int i = 0; i < row.size(); i++)
        {
            if (row[i].find(',') != string::npos)
                file << "\"" << row[i] << "\"";
            else
                file << row[i];

            file << (i == row.size() - 1 ? "" : ",");
        }
        file << "\n";
    }
    file.close();
    cout << "Data successfully saved to " << filename << endl;
}
void perform_analytics(const vector<string> &head, const vector<vector<string>> &data)
{
    cout << "1. Correlation Matrix\n2. K-Means Clustering\n3. Regression\nChoice: ";
    int ch;
    cin >> ch;
    vector<int> nums;
    for (int i = 0; i < head.size(); i++)
        if (!data.empty() && is_num(data[0][i]))
            nums.push_back(i);

    if (ch == 1)
    {
        for (int i : nums)
        {
            cout << head[i].substr(0, 5) << " | ";
            for (int j : nums)
            {
                vector<double> vx, vy;
                for (auto &r : data)
                {
                    vx.push_back(safe_stod(r[i]));
                    vy.push_back(safe_stod(r[j]));
                }
                cout << setw(8) << Analytics::calculate_correlation(vx, vy);
            }
            cout << endl;
        }
    }
    else if (ch == 2)
    {
        cout << "X col index: ";
        int x;
        cin >> x;
        cout << "Y col index: ";
        int y;
        cin >> y;
        vector<double> vx, vy;
        for (auto &r : data)
        {
            vx.push_back(safe_stod(r[x]));
            vy.push_back(safe_stod(r[y]));
        }
        Analytics::run_kmeans(vx, vy, 3);
    }
    else if (ch == 3)
    {
        cout << "\nAvailable Numeric Columns:\n";
        for (int i : nums)
            cout << i << ": " << head[i] << endl;
        cout << "Select Independent Variable (X): ";
        int x;
        cin >> x;
        cout << "Select Dependent Variable (Y to predict): ";
        int y;
        cin >> y;

        vector<double> vx, vy;
        for (auto &r : data)
        {
            if (is_num(r[x]) && is_num(r[y]))
            {
                vx.push_back(safe_stod(r[x]));
                vy.push_back(safe_stod(r[y]));
            }
        }

        cout << "Enter Train Ratio (e.g., 0.8): ";
        double ratio;
        cin >> ratio;
        pair<double, double> model = Analytics::run_regression(vx, vy, ratio);
        double slope = model.first;
        double intercept = model.second;

        if (slope != 0 || intercept != 0)
        {
            cout << "\n--- Prediction Mode (Enter -999 to exit) ---" << endl;
            double val;
            while (true)
            {
                cout << "Predict '" << head[y] << "' for '" << head[x] << "' = ";
                cin >> val;
                if (val == -999)
                    break;
                cout << ">> Result: " << slope * val + intercept << endl;
            }
        }
    }
}

void filter_data(const vector<string> &head, const vector<vector<string>> &data)
{
    cout << "Select Numeric Column to Filter (0-" << head.size() - 1 << "): ";
    int sel;
    cin >> sel;

    if (sel < 0 || sel >= (int)head.size() || !is_num(data[0][sel]))
    {
        cout << "Invalid or Non-numeric column!" << endl;
        return;
    }

    AVLTree tree;
    cout << "Building Index (AVL Tree)..." << endl;
    for (int i = 0; i < (int)data.size(); i++)
    {
        tree.add(safe_stod(data[i][sel]), i);
    }

    double minV, maxV;
    cout << "Enter Minimum Value: ";
    cin >> minV;
    cout << "Enter Maximum Value: ";
    cin >> maxV;

    cout << "\n--- Filter Results ---" << endl;
    tree.query(tree.root, minV, maxV, data);
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
    cout << "Enter the Filename";
    string fn;
    cin >> fn;
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
    while (choice != 9)
    {
        cout << "\n--- Smart Data Cleaning Engine ---" << endl;
        cout << "1. Display Current Data" << endl;
        cout << "2. Remove Columns" << endl;
        cout << "3. Handle Duplicates" << endl;
        cout << "4. Fill Missing Values" << endl;
        cout << "5. Prioritize Cleaning" << endl;
        cout << "6. Analyze Column" << endl;
        cout << "7. Filter Data" << endl;
        cout << "8. Remove row" << endl;
        cout << "9. Save data" << endl;
        cout << "10. Perform Analyics" << endl;
        cout << "11. EXIT" << endl;
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
            filter_data(head, data);
            break;
        case 8:
            remove_row(data);
            break;
        case 9:
            save_data(head, data, "cleaned_data.csv");
            break;
        case 10:
            perform_analytics(head, data);
            break;
        case 11:
            cout << "Exiting program. Goodbye!" << endl;
            return 0;
        default:
            cout << "Invalid choice! Please enter 1-11." << endl;
        }
    }
    return 0;
}