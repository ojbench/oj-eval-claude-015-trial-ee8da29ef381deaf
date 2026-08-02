#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;

const string DATA_FILE = "kv_store.dat";

struct Record {
    char key[65];
    int value;

    Record() { key[0] = '\0'; value = 0; }
    Record(const string& k, int v) {
        strncpy(key, k.c_str(), 64);
        key[64] = '\0';
        value = v;
    }

    string getKey() const { return string(key); }
};

vector<Record> records;

// Load data from disk
void loadData() {
    ifstream fin(DATA_FILE, ios::binary);
    if (!fin.is_open()) {
        return;
    }

    while (true) {
        Record rec;
        fin.read(rec.key, 65);
        if (fin.eof()) break;
        fin.read(reinterpret_cast<char*>(&rec.value), sizeof(rec.value));
        if (fin.eof()) break;
        records.push_back(rec);
    }
    fin.close();
}

// Save data to disk
void saveData() {
    ofstream fout(DATA_FILE, ios::binary | ios::trunc);
    for (const auto& rec : records) {
        fout.write(rec.key, 65);
        fout.write(reinterpret_cast<const char*>(&rec.value), sizeof(rec.value));
    }
    fout.close();
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    loadData();

    int n;
    cin >> n;

    for (int i = 0; i < n; i++) {
        string cmd;
        cin >> cmd;

        if (cmd == "insert") {
            string key;
            int value;
            cin >> key >> value;

            // Check if already exists
            bool found = false;
            for (const auto& rec : records) {
                if (rec.getKey() == key && rec.value == value) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                records.push_back(Record(key, value));
            }
        } else if (cmd == "delete") {
            string key;
            int value;
            cin >> key >> value;

            for (auto it = records.begin(); it != records.end(); ++it) {
                if (it->getKey() == key && it->value == value) {
                    records.erase(it);
                    break;
                }
            }
        } else if (cmd == "find") {
            string key;
            cin >> key;

            vector<int> values;
            for (const auto& rec : records) {
                if (rec.getKey() == key) {
                    values.push_back(rec.value);
                }
            }

            if (values.empty()) {
                cout << "null\n";
            } else {
                sort(values.begin(), values.end());
                for (size_t j = 0; j < values.size(); j++) {
                    if (j > 0) cout << " ";
                    cout << values[j];
                }
                cout << "\n";
            }
        }
    }

    saveData();

    return 0;
}
