//
//  DataSet.cpp
//  memDB
//
//  Created by Sixing Wen on 10/15/16.
//  Copyright © 2016 Sixing Wen. All rights reserved.
//

#include "DataSet.h"
/* just a simple key-value database */
void DataSet::removeKeyFromCnt(string key) {
    int originVal = data[key];
    cnt[originVal].erase(key);
    if (cnt[originVal].size() == 0) {
        cnt.erase(originVal);
    }
}
bool DataSet::ifContains(string key) {
    return data.find(key) != data.end();
}
const unordered_map<string, int> & DataSet::getData() {
        return data;
    }
bool DataSet::get(const string &key, int &value) {
        if (data.find(key) == data.end()) {
            return false;
        }
        value = data[key];
        return true;
    }
void DataSet::set(const string &key, int value) {
    if (ifContains(key) && data[key] == value) {
        return;
    }
    removeKeyFromCnt(key);
    data[key] = value;
    cnt[value].insert(key);
}
void DataSet::unset(const string &key) {
    if (!ifContains(key)) {
        return;
    }
    removeKeyFromCnt(key);
    data.erase(key);
}
int DataSet::numberEqualTo(int value) {
    int num = 0;
    if (cnt.find(value) != cnt.end()) {
        num = int(cnt[value].size());
    }
    return num;
}
const unordered_set<string> & DataSet::getCntForValue(int value) {
    static unordered_set<string> nullSet;
    if (cnt.find(value) != cnt.end()) {
        return cnt[value];
    }
    return nullSet;
}
    
void DataSet::merge(DataSet &dest, const DataSet &src) {
    for (auto itSet = src.data.begin(); itSet != src.data.end(); itSet ++) {
        dest.set(itSet->first, itSet->second);
    }
}