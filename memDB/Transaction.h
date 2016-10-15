//
//  Transaction.hpp
//  memDB
//
//  Created by Sixing Wen on 10/15/16.
//  Copyright © 2016 Sixing Wen. All rights reserved.
//

#ifndef Transaction_h
#define Transaction_h
#include "DataSet.h"
#include <memory>
#include <vector>
using namespace std;

class Transaction : public DataStorage {
private:
    typedef unordered_map<string, string> TransactionLog;
    
    DataSet &ds; // DataSet to commit changes
    shared_ptr<DataSet>   transData;
    unordered_set<string> dataToBeUnset;
    vector<TransactionLog> transLogs;
    bool ifToBeUnset(const string &key);
    void addLog(string key, string value);
    void doSet(const string &key, int value);
    void doUnset(const string &key);
    void clear();
public:
    Transaction(DataSet &ds);
    
    virtual bool get(const string &key, int &value);
    
    virtual void set(const string &key, int value);
    
    virtual void unset(const string &key);
    
    virtual int numberEqualTo(int value);
    
    void startNew();
    
    void rollback();
    
    void commit();
};

#endif /* Transaction_h */
