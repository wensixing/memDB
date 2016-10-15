//
//  DataStorage.h
//  memDB
//
//  Created by Sixing Wen on 10/15/16.
//  Copyright © 2016 Sixing Wen. All rights reserved.
//

#ifndef DataStorage_h
#define DataStorage_h
#include <string>
using namespace std;

class DataStorage {
public:
    virtual ~DataStorage() {}
    virtual bool get(const string &key, int &value) = 0; // false -- key doesn't exist
    virtual void set(const string &key, int value) = 0;
    virtual void unset(const string &key) = 0;
    virtual int  numberEqualTo(int value) = 0;
};

#endif /* DataStorage_h */
