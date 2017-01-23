
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include <string>

using namespace std;

// gets the i^th page in the table whichTable... note that if the page
// is currently being used (that is, the page is current buffered) a handle
// to that already-buffered page should be returned
MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr whichTable, long i) {
	
    //is page currently in the buffer?
    pair<string,long> key = new pair<string,long>(*whichTable.getName(),i);
    std::map<pair<string,long>,sharedPtr<Page>>::iterator it;
    it = IDTable.find(key);
    if (it == IDTable.end()) {
        // not found
        //allocate a free page in the buffer to this page and create a new handle and return handle
        
    } else {
        // found
        //create new PageHandle
        MyDB_PageHandle handle = new MyDB_PageHandle(it->second);
    }
    
    //is i valid?
    //string& page = whichTable.getStorageLoc() + i * this.pageSize;
    int fd = open (whichTable->getStorageLoc ().c_str (), O_CREAT | O_RDWR | O_SYNCH, 0666);
    off_t temp = lseek(fd, i * this.pageSize,  SEEK_CUR);
    read(fd, )
    
    
    //if yes return a handle in the buffer
    
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
    this.pageSize = pageSize;
    this.numPages = numPages;
    this.tempFile = tempFile;
    this.IDTable = new map<pair<string, long>, shared_ptr<char>>();
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
}
	
#endif


