#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace std;

const string DATA_FILE = "kv_store.dat";

unordered_map<string, vector<int>> dataMap;

void loadData() {
    ifstream fin(DATA_FILE, ios::binary);
    if (!fin.is_open()) {
        return;
    }

    while (true) {
        int keyLen;
        fin.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
        if (fin.eof() || keyLen <= 0 || keyLen > 64) break;

        char keyBuf[65];
        fin.read(keyBuf, keyLen);
        if (fin.eof()) break;
        keyBuf[keyLen] = '\0';

        int value;
        fin.read(reinterpret_cast<char*>(&value), sizeof(value));
        if (fin.eof()) break;

        dataMap[string(keyBuf)].push_back(value);
    }
    fin.close();

    // Sort values for each key
    for (auto& entry : dataMap) {
        sort(entry.second.begin(), entry.second.end());
    }
}

void saveData() {
    ofstream fout(DATA_FILE, ios::binary | ios::trunc);

    for (const auto& entry : dataMap) {
        const string& key = entry.first;
        for (int value : entry.second) {
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

            auto& values = dataMap[key];
            // Use binary search since vector is kept sorted
            auto it = lower_bound(values.begin(), values.end(), value);
            if (it == values.end() || *it != value) {
                values.insert(it, value);
            }
        } else if (cmd == "delete") {
            string key;
            int value;
            cin >> key >> value;

            auto mapIt = dataMap.find(key);
            if (mapIt != dataMap.end()) {
                auto& values = mapIt->second;
                auto it = lower_bound(values.begin(), values.end(), value);
                if (it != values.end() && *it == value) {
                    values.erase(it);
                    if (values.empty()) {
                        dataMap.erase(mapIt);
                    }
                }
            }
        } else if (cmd == "find") {
            string key;
            cin >> key;

            auto it = dataMap.find(key);
            if (it == dataMap.end() || it->second.empty()) {
                cout << "null\n";
            } else {
                const auto& values = it->second;
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
