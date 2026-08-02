#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

const string DATA_FILE = "kv_store.dat";

map<string, set<int>> dataMap;

void loadData() {
    ifstream fin(DATA_FILE, ios::binary);
    if (!fin.is_open()) {
        return;
    }

    while (true) {
        unsigned char keyLen;
        fin.read(reinterpret_cast<char*>(&keyLen), 1);
        if (fin.eof() || keyLen == 0 || keyLen > 64) break;

        char keyBuf[65];
        fin.read(keyBuf, keyLen);
        if (fin.eof()) break;

        int value;
        fin.read(reinterpret_cast<char*>(&value), sizeof(value));
        if (fin.eof()) break;

        dataMap[string(keyBuf, keyLen)].insert(value);
    }
    fin.close();
}

void saveData() {
    ofstream fout(DATA_FILE, ios::binary | ios::trunc);

    for (const auto& entry : dataMap) {
        const string& key = entry.first;
        unsigned char keyLen = key.length();

        for (int value : entry.second) {
            fout.write(reinterpret_cast<const char*>(&keyLen), 1);
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

        if (cmd[0] == 'i') {  // insert
            string key;
            int value;
            cin >> key >> value;
            dataMap[key].insert(value);
        } else if (cmd[0] == 'd') {  // delete
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
        } else {  // find
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

    saveData();

    return 0;
}
