
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "MyDB_PageHandle.h"

void *MyDB_PageHandleBase :: getBytes () {
	//check if data is in buffer
    //if(this->pagePtr->inBuffer != true){
    //}
    //return pointer to data in buffer
    this->pagePtr->updateLRU(this->pagePtr);
    return this->pagePtr->pageAddress;
}

void MyDB_PageHandleBase :: wroteBytes () {
    this->pagePtr->dirtyBit = true;
    this->pagePtr->updateLRU(this->pagePtr);
}

MyDB_PageHandleBase :: MyDB_PageHandleBase (MyDB_Page * page) {
    this->pagePtr = page;
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
    if(this->pagePtr->pinned){
        if(this->pagePtr->refCount <= 1)
            this->pagePtr->pinned = false;
    }
}

#endif

