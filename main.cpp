#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

const string DATA_FILE = "kv_store.dat";

// In-memory data structure
map<string, set<int>> dataMap;

// Load data from disk
void loadData() {
    ifstream fin(DATA_FILE, ios::binary);
    if (!fin.is_open()) {
        return; // File doesn't exist yet
    }

    while (fin.peek() != EOF) {
        // Read key length
        int keyLen;
        fin.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
        if (fin.eof()) break;

        // Read key
        char keyBuf[65];
        fin.read(keyBuf, keyLen);
        keyBuf[keyLen] = '\0';
        string key(keyBuf);

        // Read value
        int value;
        fin.read(reinterpret_cast<char*>(&value), sizeof(value));

        dataMap[key].insert(value);
    }
    fin.close();
}

// Save data to disk
void saveData() {
    ofstream fout(DATA_FILE, ios::binary | ios::trunc);

    for (const auto& entry : dataMap) {
        const string& key = entry.first;
        const set<int>& values = entry.second;

        for (int value : values) {
            int keyLen = key.length();
            fout.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));
            fout.write(key.c_str(), keyLen);
            fout.write(reinterpret_cast<const char*>(&value), sizeof(value));
        }
    }

    fout.close();
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Load existing data
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
            dataMap[key].insert(value);
        } else if (cmd == "delete") {
            string key;
            int value;
            cin >> key >> value;
            auto it = dataMap.find(key);
            if (it != dataMap.end()) {
                it->second.erase(value);
                if (it->second.empty()) {
                    dataMap.erase(it);
                }
            }
        } else if (cmd == "find") {
            string key;
            cin >> key;
            auto it = dataMap.find(key);
            if (it == dataMap.end() || it->second.empty()) {
                cout << "null\n";
            } else {
                bool first = true;
                for (int value : it->second) {
                    if (!first) cout << " ";
                    cout << value;
                    first = false;
                }
                cout << "\n";
            }
        }
    }

    // Save data back to disk
    saveData();

    return 0;
}
