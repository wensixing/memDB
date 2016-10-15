//
//  DataSet.hpp
//  memDB
//
//  Created by Sixing Wen on 10/15/16.
//  Copyright © 2016 Sixing Wen. All rights reserved.
//

#ifndef DataSet_h
#define DataSet_h
#include "DataStorage.h"
#include <unordered_map>
#include <unordered_set>
using namespace std;
class DataSet : public DataStorage {
private:
    unordered_map<string, int> data;
    unordered_map<int, unordered_set<string>> cnt;
    void removeKeyFromCnt(string key);
    bool ifContains(string key);
public:
    const unordered_map<string, int> &getData();
    virtual bool get(const string &key, int &value);
    virtual void set(const string &key, int value);
    virtual void unset(const string &key);
    virtual int numberEqualTo(int value);
    const unordered_set<string> &getCntForValue(int value);
    
    // merge data in 'src' into 'dest' (update if exists, insert if not)
    static void merge(DataSet &dest, const DataSet &src);
    
};

#endif /* DataSet_hpp */
