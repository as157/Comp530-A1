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
class MyDB_BufferManager;
//#include "MyDB_BufferManager.h"

using namespace std;

class MyDB_Page{
    
public:
    //should always be constructed with parameters
    MyDB_Page(char* addr, MyDB_BufferManager * bfRef, bool pinned, bool anon, MyDB_TablePtr whichTable, long i) ;
    
    //memory address in buffer memory
    char* pageAddress;
    
    //reference back to buffer manager. Required to alert buffer manager when destructed
    MyDB_BufferManager *  bufferManagerRef;
    
    //pinned in buffer, default = false
    bool pinned;
    
    //anonymous page = true or one associated with DB table = false
    bool anon;

    //associated DB table, if annonymous = null
    MyDB_TablePtr whichTable;
    long offset;
 
    //is page in buffer dirty, default = false
    bool dirtyBit;
    
    //increments when pageHandle method getBytes(), wroteBytes() is called; default = 0
    int refCount;
    
    //flag to indicate if in buffer; default = true
    bool inBuffer;
    
    ~MyDB_Page();
    
    void updateLRU(MyDB_Page * pagePtr);
    
};

#endif
