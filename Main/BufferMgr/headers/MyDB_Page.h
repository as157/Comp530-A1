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
    ~MyDB_Page();
    
    
    
private:
    
    int LRUPos;
    int refCount;
    int dirtyBit;
    
    
};

#endif
