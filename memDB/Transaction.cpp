//
//  Transaction.cpp
//  memDB
//
//  Created by Sixing Wen on 10/15/16.
//  Copyright © 2016 Sixing Wen. All rights reserved.
//

#include "Transaction.h"
bool Transaction::ifToBeUnset(const string &key) {
    return dataToBeUnset.find(key) != dataToBeUnset.end();
}

void Transaction::addLog(string key, string value) {
    if (transLogs.back().find(key) != transLogs.back().end()) {
        return;
    }
    transLogs.back()[key] = value;
}

void Transaction::doSet(const string &key, int value) {
    transData->set(key, value);
    if (ifToBeUnset(key)) {
        dataToBeUnset.erase(key);
    }
}

void Transaction::doUnset(const string &key) {
    transData->unset(key);
    if (!ifToBeUnset(key)) {
        dataToBeUnset.insert(key);
    }
}

void Transaction::clear() {
    transData = make_shared<DataSet>();
    dataToBeUnset.clear();
    transLogs.clear();
    startNew();
}
Transaction::Transaction(DataSet &ds) : ds(ds) {
    clear();
}

bool Transaction::get(const string &key, int &value) {
    if (ifToBeUnset(key)) {
        return false;
    }
    if (transData->get(key, value)) {
        return true;
    }
    return ds.get(key, value);
}

void Transaction::set(const string &key, int value) {
    // add log
    int originValue;
    if (get(key, originValue)) {
        addLog(key, to_string(originValue));
    } else {
        addLog(key, "NULL");
    }
    
    doSet(key, value);
}

void Transaction::unset(const string &key) {
    // add log
    int originValue;
    if (get(key, originValue)) {
        addLog(key, to_string(originValue));
    }
    
    doUnset(key);
}

int Transaction::numberEqualTo(int value) {
    const unordered_map<string, int> &trData = transData->getData();
    const unordered_set<string> &current = ds.getCntForValue(value);
    int len = int(current.size());
    for (auto it = trData.begin(); it != trData.end(); it ++) {
        if (current.find(it->first) != current.end()) {
            len --;
        }
    }
    for (auto it = dataToBeUnset.begin(); it != dataToBeUnset.end(); it ++) {
        if (current.find(*it) != current.end()) {
            len --;
        }
    }
    return len + transData->numberEqualTo(value);
}

void Transaction::startNew() {
    TransactionLog newlog;
    transLogs.push_back(newlog);
}

void Transaction::rollback() {
    TransactionLog &log = transLogs.back();
    
    for (auto it = log.begin(); it != log.end(); it ++) {
        if (it->second == "NULL") {
            doUnset(it->first);
        } else {
            doSet(it->first,stoi(it->second));
        }
    }
    
    transLogs.pop_back();
}

void Transaction::commit() {
        // update data
        DataSet::merge(ds, *transData);
        
        unordered_set<string>& unsetData     = dataToBeUnset;
        unordered_set<string>::iterator itUnset;
        for (itUnset = unsetData.begin(); itUnset != unsetData.end(); itUnset ++) {
            ds.unset(*itUnset);
        }
        
        clear();
    }
