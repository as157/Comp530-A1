
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

// gets the i^th page in the table whichTable... note that if the page
// is currently being used (that is, the page is current buffered) a handle
// to that already-buffered page should be returned
MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr whichTable, long i) {
	
    //is page currently in the buffer?
    pair<string,long> key(whichTable->getName(),i);
    auto it = pageTable.find(key);
    
    if (it == pageTable.end()) {
        // not found in buffer therefore in DB table
        //check for empty spot in buffer
        char* newPage;
        if(this->pageTable.size() != this->pageSize){
            newPage = &this->buffer[this->pageSize * this->pageTable.size()-1];
        }
        
        //otherwise evict a page
        else{
            
        }
        
        // read data into the buffer
        int fd = open (whichTable->getStorageLoc ().c_str (), O_CREAT|O_RDWR|O_SYNC, 0666);
        lseek(fd, i * this->pageSize,  SEEK_CUR);
        read(fd, newPage, this->pageSize);
        
        // remove from pageTable. Check dirty bit. If dirty writeback to db table. If not dirty just delete somehow. What happens to any pagehandles in the system that currently reference that page in buffer?
        // if yes create a new page handle and place in pageTable
        // if no evict LRU page
        
        
    } else {
        // found
        //create new PageHandle and return
        MyDB_PageHandle handle = make_shared<MyDB_PageHandleBase>(shared_ptr<MyDB_Page>(it->second));
        //******************increment LRU number
        
        return handle;
    }
    
    return nullptr;
}

// gets a temporary page that will no longer exist (1) after the buffer manager
// has been destroyed, or (2) there are no more references to it anywhere in the
// program.  Typically such a temporary page will be used as buffer memory.
// since it is just a temp page, it is not associated with any particular
// table
MyDB_PageHandle MyDB_BufferManager :: getPage () {
	return nullptr;		
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr, long) {
	return nullptr;		
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
	return nullptr;		
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile) {
    this->pageSize = pageSize;
    this->numPages = numPages;
    this->tempFile = tempFile;
    
    //create buffer
    this->buffer = (char*) malloc(this->pageSize * this->numPages);
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
}
	
#endif


