
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
        // check for availability in buffer
        if(!this->bufferQ.empty()){
            //get available address in buffer
            char* newAddr = this->bufferQ.front();
            this->bufferQ.pop();
            
            //create new page and add to pageTable
            MyDB_Page newPage(newAddr);
            this->pageTable.insert({key, make_shared<MyDB_Page>(newPage)});
            
            // read data into the buffer at page address
            int fd = open (whichTable->getStorageLoc ().c_str (), O_CREAT|O_RDWR|O_SYNC, 0666);
            lseek(fd, i * this->pageSize,  SEEK_CUR);
            read(fd, newAddr, this->pageSize);
            assert(close(fd));
            
            //create pagehandle and return
            MyDB_PageHandle handle = make_shared<MyDB_PageHandleBase>(make_shared<MyDB_Page>(newPage));
            return handle;
        }
        
        //otherwise evict a page
        else{
            evictNode();
            // the destructor of the page object should place the address back in the buffer so this case is handled at the beginning of this function. Therefore call getPage(table, i) again
            getPage(whichTable, i);
        }
        
    } else {
        // found
        //create new PageHandle and return
        MyDB_PageHandle handle = make_shared<MyDB_PageHandleBase>(it->second);
        
        // remove node from list and reinsert at end of list to update LRU
        Node * node = removeNode(it->second);
        insertNode(node);
        
        return handle;
    }
    
    return nullptr;
}

void MyDB_BufferManager :: evictNode(){
    // first find node in list that contains page that is not pinned
    Node * evictNode = getNextNode();
    
    //get reference page and delete node
    shared_ptr<MyDB_Page> pageRef = evictNode->pageRef;
    delete evictNode;
    
    // check dirty bit. If dirty writeback to db table or tempfile.
    if(pageRef->dirtyBit == true){
        if(pageRef->anon == false){
            int fd = open (pageRef->whichTable->getStorageLoc ().c_str (), O_CREAT|O_RDWR|O_SYNC, 0666);
            lseek(fd, pageRef->offset * this->pageSize,  SEEK_CUR);
            write(fd, pageRef->pageAddress, this->pageSize);
            assert(close(fd));
            
            //delete page object
            pageRef.reset();
            
        }
        else{
            //anonymous page
            std::cout<<"this case should never happen inside getPage(table,i)"<<endl;
            
        }
    }
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
        head->prev = NULL;
    }
    else{
        currentNode->prev->next = currentNode->next;
        currentNode->next->prev = currentNode->prev;
    }
    
    return currentNode;
}

// find and remove node from list
Node* MyDB_BufferManager :: removeNode(shared_ptr<MyDB_Page> page){
    Node * previousNode = this->head;
    Node * currentNode = this->head;
    while(currentNode != NULL){
        if(currentNode->pageRef == page){
            previousNode->next = currentNode->next;
            currentNode->next->prev = previousNode;
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
    if(this->head == NULL){
        n->prev = NULL;
        n->next = NULL;
        this->head = n;
        this->end = n;
    }
    else{
        n->next = NULL;
        n->prev = this->end;
        this->end->next = n;
        this->end = n;
    }
}

void MyDB_BufferManager :: deletePage(char* addr, pair<string,int> key){
    this->bufferQ.push(addr);
    this->pageTable.erase(key);
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
    this->head->prev = NULL;
    this->head->next = NULL;
    this->head = NULL;
    this->end = this->head;
    
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
}

MyDB_Page :: ~MyDB_Page () {
    pair<string,long> key(this->whichTable->getName(),this->offset);
    this->bufferManagerRef->deletePage(this->pageAddress, key);
    delete this->pageAddress;
}

#endif


