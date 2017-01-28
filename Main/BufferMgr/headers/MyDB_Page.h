//
//  MyDB_Page.h
//  A1
//
//  Created by Adrian Smart on 1/22/17.
//  Copyright © 2017 Adrian Smart. All rights reserved.
//

#ifndef MyDB_Page_h
#define MyDB_Page_h
#include "MyDB_Table.h"

using namespace std;

class MyDB_Page{
    
public:
    
    MyDB_Page();
    MyDB_Page(char* addr);
    MyDB_TablePtr whichTable;
    long offset;
    bool anon;
    bool pinned;
    bool dirtyBit;
    int refCount;
    char* pageAddress;
    ~MyDB_Page();

private:
    
    
    
    
};

#endif
