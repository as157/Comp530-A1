//
//  MyDB_Page.cpp
//  A1
//
//  Created by Adrian Smart on 1/22/17.
//  Copyright © 2017 Adrian Smart. All rights reserved.
//

#include <stdio.h>
#include "MyDB_Page.h"
//class MyDB_BufferManager;


MyDB_Page :: MyDB_Page(char* addr, MyDB_BufferManager * bfRef, bool pinned, bool anon, MyDB_TablePtr whichTable, long i) {
    this->pageAddress = addr;
    this->bufferManagerRef = bfRef;
    this->pinned = pinned;
    this->anon = anon;
    this->whichTable = whichTable;
    this->offset = i;
    this->dirtyBit = false;
    this->refCount = 0;
}

//MyDB_Page :: ~MyDB_Page () {
//    pair<string,long> key(this->whichTable->getName(),this->offset);
//    char* temp = NULL;
    
//    this->bufferManagerRef->deletePage(temp, key);
//}

