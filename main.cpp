#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;

const int NUM_BUCKETS = 16;  // Use 16 bucket files to distribute data
const string FILE_PREFIX = "db_";

// Hash function for string keys
size_t hashKey(const string& key) {
    size_t hash = 0;
    for (char c : key) {
        hash = hash * 31 + c;
    }
    return hash % NUM_BUCKETS;
}

// Get filename for a bucket
string getBucketFile(size_t bucket) {
    return FILE_PREFIX + to_string(bucket) + ".dat";
}

// Insert a key-value pair
void insertKV(const string& key, int value) {
    size_t bucket = hashKey(key);
    string filename = getBucketFile(bucket);

    // Check if this exact pair already exists
    ifstream fin(filename, ios::binary);
    if (fin.is_open()) {
        while (fin.peek() != EOF) {
            // Read key length
            int keyLen;
            fin.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
            if (fin.eof()) break;

            // Read key
            char* keyBuf = new char[keyLen + 1];
            fin.read(keyBuf, keyLen);
            keyBuf[keyLen] = '\0';
            string existingKey(keyBuf);
            delete[] keyBuf;

            // Read value
            int existingValue;
            fin.read(reinterpret_cast<char*>(&existingValue), sizeof(existingValue));

            // Read deleted flag
            char deleted;
            fin.read(&deleted, sizeof(deleted));

            // If same key-value pair exists (not deleted), don't insert
            if (existingKey == key && existingValue == value && deleted == 0) {
                fin.close();
                return;
            }
        }
        fin.close();
    }

    // Append new record
    ofstream fout(filename, ios::binary | ios::app);
    int keyLen = key.length();
    fout.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));
    fout.write(key.c_str(), keyLen);
    fout.write(reinterpret_cast<const char*>(&value), sizeof(value));
    char deleted = 0;
    fout.write(&deleted, sizeof(deleted));
    fout.close();
}

// Delete a key-value pair
void deleteKV(const string& key, int value) {
    size_t bucket = hashKey(key);
    string filename = getBucketFile(bucket);

    ifstream fin(filename, ios::binary);
    if (!fin.is_open()) {
        return;  // File doesn't exist, nothing to delete
    }

    // Read all records
    vector<pair<streampos, bool>> recordPositions;
    while (fin.peek() != EOF) {
        streampos pos = fin.tellg();

        // Read key length
        int keyLen;
        fin.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
        if (fin.eof()) break;

        // Read key
        char* keyBuf = new char[keyLen + 1];
        fin.read(keyBuf, keyLen);
        keyBuf[keyLen] = '\0';
        string existingKey(keyBuf);
        delete[] keyBuf;

        // Read value
        int existingValue;
        fin.read(reinterpret_cast<char*>(&existingValue), sizeof(existingValue));

        // Read deleted flag
        char deleted;
        fin.read(&deleted, sizeof(deleted));

        // If this is the record to delete, mark its position
        if (existingKey == key && existingValue == value && deleted == 0) {
            recordPositions.push_back({pos, true});
        } else {
            recordPositions.push_back({pos, false});
        }
    }
    fin.close();

    // Mark records as deleted by updating the deleted flag
    for (auto& rec : recordPositions) {
        if (rec.second) {
            fstream fio(filename, ios::binary | ios::in | ios::out);

            // Calculate position of deleted flag
            fio.seekg(rec.first);

            // Read key length to skip it
            int keyLen;
            fio.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));

            // Skip key and value
            fio.seekg(keyLen + sizeof(int), ios::cur);

            // Write deleted flag
            char deleted = 1;
            fio.write(&deleted, sizeof(deleted));
            fio.close();
            break;  // Only delete first occurrence
        }
    }
}

// Find all values for a key
void findKV(const string& key) {
    size_t bucket = hashKey(key);
    string filename = getBucketFile(bucket);

    vector<int> values;

    ifstream fin(filename, ios::binary);
    if (fin.is_open()) {
        while (fin.peek() != EOF) {
            // Read key length
            int keyLen;
            fin.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
            if (fin.eof()) break;

            // Read key
            char* keyBuf = new char[keyLen + 1];
            fin.read(keyBuf, keyLen);
            keyBuf[keyLen] = '\0';
            string existingKey(keyBuf);
            delete[] keyBuf;

            // Read value
            int existingValue;
            fin.read(reinterpret_cast<char*>(&existingValue), sizeof(existingValue));

            // Read deleted flag
            char deleted;
            fin.read(&deleted, sizeof(deleted));

            // If key matches and not deleted, add to results
            if (existingKey == key && deleted == 0) {
                values.push_back(existingValue);
            }
        }
        fin.close();
    }

    if (values.empty()) {
        cout << "null" << endl;
    } else {
        sort(values.begin(), values.end());
        for (size_t i = 0; i < values.size(); i++) {
            if (i > 0) cout << " ";
            cout << values[i];
        }
        cout << endl;
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
