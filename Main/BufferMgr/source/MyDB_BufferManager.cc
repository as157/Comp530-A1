
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

using namespace std;

// gets the i^th page in the table whichTable... note that if the page
// is currently being used (that is, the page is current buffered) a handle
// to that already-buffered page should be returned
MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr whichTable, long i) {
	
    //is page currently in the buffer?
    pair<string,long> key(whichTable->getName(),i);
    auto it = this->pageTable.find(key);
    
    if (it == this->pageTable.end()) {
        // not found in pageTable therefore allocate a new Page
        // check for availability in address buffer
        if(!this->bufferQ.empty()){
            //get available address in buffer
            char * newAddr = getNewBufferAddress();
            
            //create new page and set fields
            //MyDB_Page newPage(newAddr, this, true, false, whichTable, i);
            
            //add to pageTable and insert in LRU linked list
            this->pageTable.insert({key, new MyDB_Page(newAddr, this, false, false, whichTable, i)});
            insertNode(new Node(this->pageTable[key]));
            
            // read data into the buffer at page address
            readDataIntoBuffer(newAddr, whichTable, i);
            
            //create pagehandle and return
            MyDB_PageHandle handle = make_shared<MyDB_PageHandleBase>(this->pageTable[key]);
            return handle;
        }
        
        //otherwise evict a page
        else{
            evictNode();
            // the destructor of the page object should place the address back in the buffer so this case is handled at the beginning of this function. Therefore call getPage(table, i) again
            return getPage(whichTable, i);
        }
        
    } else {
        // found
        //create new PageHandle and return
        MyDB_PageHandle handle = make_shared<MyDB_PageHandleBase>(it->second);
        
        //update LRU
        updateLRU(it->second);
        
        Node * node = removeNode(it->second);
        insertNode(node);
        
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
    //get new page from bufferQ
    if(!this->bufferQ.empty()){
        //get available address in buffer
        char * newAddr = getNewBufferAddress();
        
        //create new page and set fields
        //MyDB_Page newPage(newAddr, this, false, false, NULL, NULL);
        
        //create pagehandle and return
        MyDB_PageHandle handle = make_shared<MyDB_PageHandleBase>(new MyDB_Page(newAddr, this, true, true, nullptr, NULL));
        insertNode(new Node(handle->pagePtr));
        return handle;
        
    }
    //evict a current page in buffer
    else{
        evictNode();
        // the destructor of the page object should place the address back in the buffer so this case is handled at the beginning of this function. Therefore call getPage(table, i) again
        return getPage();
    }
    return nullptr;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr whichTable, long i) {
    //is pinned page currently in the buffer?
    pair<string,long> key(whichTable->getName(),i);
    auto it = this->pageTable.find(key);
    
    if (it == this->pageTable.end()) {
        // not found in pageTable therefore allocate a new Page
        // check for availability in address buffer
        if(!this->bufferQ.empty()){
            //get available address in buffer
            char * newAddr = getNewBufferAddress();
            
            //create new page and set fields
            //MyDB_Page newPage(newAddr, this, true, false, whichTable, i);
            
            //add to pageTable
            this->pageTable.insert({key, new MyDB_Page(newAddr, this, true, false, whichTable, i)});
            insertNode(new Node(this->pageTable[key]));
            
            // read data into the buffer at page address
            readDataIntoBuffer(newAddr, whichTable, i);
            
            //create pagehandle and return
            //MyDB_Page newPage = this->pageTable[key];
            MyDB_PageHandle handle = make_shared<MyDB_PageHandleBase>(this->pageTable[key]);
            handle->pagePtr->refCount++;
            return handle;
        }
        
        //otherwise evict a page
        else{
            evictNode();
            // the destructor of the page object should place the address back in the buffer so this case is handled at the beginning of this function. Therefore call getPage(table, i) again
            return getPinnedPage(whichTable, i);
        }
        
    } else {
        // found
        //create new PageHandle and return
        MyDB_PageHandle handle = make_shared<MyDB_PageHandleBase>(it->second);
        
        //update LRU
        updateLRU(it->second);
        
        return handle;
    }
    return nullptr;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
    //get new page from bufferQ
    if(!this->bufferQ.empty()){
        //get available address in buffer
        char * newAddr = getNewBufferAddress();
        
        //create new page and set fields
        //MyDB_Page newPage(newAddr, this, true, false, NULL, NULL);
        
        //create pagehandle and return
        MyDB_PageHandle handle = make_shared<MyDB_PageHandleBase>(new MyDB_Page (newAddr, this, true, true, nullptr, NULL));
        insertNode(new Node(handle->pagePtr));
        handle->pagePtr->refCount++;
        return handle;
        
    }
    //evict a current page in buffer
    else{
        evictNode();
        // the destructor of the page object should place the address back in the buffer so this case is handled at the beginning of this function. Therefore call getPage(table, i) again
        return getPinnedPage();
    }
    return nullptr;
}

char* MyDB_BufferManager :: getNewBufferAddress(){
    char* newAddr = this->bufferQ.front();
    this->bufferQ.pop();
    return newAddr;
}

// remove node from list and reinsert at end of list to update LRU
void MyDB_BufferManager :: updateLRU(MyDB_Page * pg){
    Node * node = removeNode(pg);
    insertNode(node);
}

void MyDB_BufferManager :: readDataIntoBuffer(char* addr, MyDB_TablePtr whichTable, long i){
    int fd = open (whichTable->getStorageLoc ().c_str (), O_CREAT|O_RDWR|O_SYNC, 0666);
    lseek(fd, i * this->pageSize,  SEEK_CUR);
    read(fd, addr, this->pageSize);
    std::cout<<"closing file: %i"<<close(fd)<<endl;
    //assert(close(fd));
}

bool MyDB_BufferManager :: evictNode(){
    // first find node in list that contains page that is not pinned
    Node * evictNode = getNextNode();
    
    if(evictNode == NULL)
        return false;
    
    //get reference page and delete node
    MyDB_Page * pageRef = evictNode->pageRef;
    delete evictNode;
    
    // check dirty bit. If dirty writeback to db table or tempfile.
    if(pageRef->dirtyBit == true){
        if(pageRef->anon == false){
            int fd = open (pageRef->whichTable->getStorageLoc ().c_str (), O_CREAT|O_RDWR|O_SYNC, 0666);
            lseek(fd, pageRef->offset * this->pageSize,  SEEK_CUR);
            write(fd, pageRef->pageAddress, this->pageSize);
            close(fd);
            //assert(close(fd));
        }
        else{
            //anonymous page so write back to tempFile
            int fd = open (this->tempFile.c_str (), O_CREAT|O_RDWR|O_SYNC, 0666);
            lseek(fd, this->tempFileOffset * this->pageSize,  SEEK_CUR);
            write(fd, pageRef->pageAddress, this->pageSize);
            close(fd);
        }
        
    }
    
    //delete page object
    delete pageRef;
    
    return true;
}

// get next available node
Node* MyDB_BufferManager :: getNextNode(){
    Node * currentNode = this->head;
    while(currentNode != NULL && currentNode->pageRef->pinned){
        currentNode = currentNode->next;
    }
    if(currentNode == NULL)
        return NULL;
    if(currentNode == head){
        head = currentNode->next;
        if(head != NULL)
            head->prev = NULL;
    }
    else{
        currentNode->prev->next = currentNode->next;
        if(currentNode->next != NULL)
            currentNode->next->prev = currentNode->prev;
    }
    
    return currentNode;
}

// find and remove node from list
Node* MyDB_BufferManager :: removeNode(MyDB_Page * page){
    Node * previousNode = this->head;
    Node * currentNode = this->head;
    if(this->head == NULL)
        return NULL;
    if(page == head->pageRef){
        this->head = currentNode->next;
        if(this->head != NULL)
            this->head->prev = NULL;
        else
            this->end = NULL;
        return currentNode;
    }
    while(currentNode != NULL){
        if(currentNode->pageRef == page){
            previousNode->next = currentNode->next;
            if(currentNode->next != NULL)
                currentNode->next->prev = previousNode;
            else
                this->end = previousNode;
            return currentNode;
        }
        previousNode = currentNode;
        currentNode = currentNode->next;
    }
    return NULL;
}

// append node to end of list
void MyDB_BufferManager :: insertNode(Node* n){
    if(n == NULL)
        return;
    //empty list
    if(this->end == NULL && this->head == NULL){
        this->head = n;
        this->head->prev = NULL;
        this->head->next = NULL;
        this->end = head;
    }
    else{
       
        
        n->next = NULL;
        n->prev = this->end;
        this->end->next = n;
        this->end = n;
    }
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
    //page should already be in LRU linked list but need to change flag on page object
    unpinMe->pagePtr->pinned = false;
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile) {
    this->pageSize = pageSize;
    this->numPages = numPages;
    this->tempFile = tempFile;
    
    
    //create buffer
    this->buffer = (char*) malloc(this->pageSize * this->numPages);
    
    //populate bufferQ
    for(int i = 0; i < numPages; i++){
        this->bufferQ.push((this->buffer + i*this->pageSize));
    }
    
    //initialize linked list
    this->head = NULL;
    this->end = NULL;
    
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
    //first unpin all pages in buffer
    Node * currentNode = head;
    while(currentNode != NULL){
        currentNode->pageRef->pinned = false;
        currentNode = currentNode->next;
    }
    
    //next evict all pages
    while(evictNode());
    
    //delete temp files
    
    //cleanup
    while(!this->bufferQ.empty()){
        this->bufferQ.pop();
    }
    free(this->buffer);
    
}

void MyDB_BufferManager :: addAddressToBufferQ(char* addr){
    this->bufferQ.push(addr);
}

void MyDB_BufferManager :: deletePage(char* addr, pair<string,int> key){
    this->pageTable.erase(key);
    //this->bufferQ.push(addr);
}

MyDB_Page :: ~MyDB_Page () {
    this->bufferManagerRef->addAddressToBufferQ(this->pageAddress);
    if(this->anon == false){
        pair<string,long> key(this->whichTable->getName(),this->offset);
        this->bufferManagerRef->deletePage(this->pageAddress, key);
    }
    
}

void MyDB_Page :: updateLRU(MyDB_Page * pagePtr){
    this->bufferManagerRef->updateLRU(pagePtr);
}



#endif


