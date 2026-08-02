#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;

const string DATA_FILE = "kv_store.dat";

struct Record {
    int keyLen;
    char key[65];
    int value;
    char deleted;
};

void insertKV(const string& key, int value) {
    // First check if it already exists
    ifstream fin(DATA_FILE, ios::binary);
    if (fin.is_open()) {
        Record rec;
        while (fin.read(reinterpret_cast<char*>(&rec), sizeof(Record))) {
            string existingKey(rec.key, rec.keyLen);
            if (existingKey == key && rec.value == value && !rec.deleted) {
                fin.close();
                return; // Already exists
            }
        }
        fin.close();
    }

    // Append new record
    ofstream fout(DATA_FILE, ios::binary | ios::app);
    Record rec;
    rec.keyLen = key.length();
    memcpy(rec.key, key.c_str(), rec.keyLen);
    memset(rec.key + rec.keyLen, 0, 65 - rec.keyLen);
    rec.value = value;
    rec.deleted = 0;
    fout.write(reinterpret_cast<const char*>(&rec), sizeof(Record));
    fout.close();
}

void deleteKV(const string& key, int value) {
    fstream fio(DATA_FILE, ios::binary | ios::in | ios::out);
    if (!fio.is_open()) return;

    Record rec;
    while (fio.read(reinterpret_cast<char*>(&rec), sizeof(Record))) {
        string existingKey(rec.key, rec.keyLen);
        if (existingKey == key && rec.value == value && !rec.deleted) {
            // Mark as deleted
            streampos pos = fio.tellg();
            pos -= static_cast<streamoff>(sizeof(Record));
            fio.seekp(pos + static_cast<streamoff>(offsetof(Record, deleted)));
            char deleted = 1;
            fio.write(&deleted, sizeof(deleted));
            fio.close();
            return;
        }
    }
    fio.close();
}

void findKV(const string& key) {
    vector<int> values;

    ifstream fin(DATA_FILE, ios::binary);
    if (fin.is_open()) {
        Record rec;
        while (fin.read(reinterpret_cast<char*>(&rec), sizeof(Record))) {
            string existingKey(rec.key, rec.keyLen);
            if (existingKey == key && !rec.deleted) {
                values.push_back(rec.value);
            }
        }
        fin.close();
    }

    if (values.empty()) {
        cout << "null\n";
    } else {
        sort(values.begin(), values.end());
        for (size_t i = 0; i < values.size(); i++) {
            if (i > 0) cout << " ";
            cout << values[i];
        }
        cout << "\n";
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    for (int i = 0; i < n; i++) {
        string cmd;
        cin >> cmd;

        if (cmd == "insert") {
            string key;
            int value;
            cin >> key >> value;
            insertKV(key, value);
        } else if (cmd == "delete") {
            string key;
            int value;
            cin >> key >> value;
            deleteKV(key, value);
        } else if (cmd == "find") {
            string key;
            cin >> key;
            findKV(key);
        }
    }

    return 0;
}
