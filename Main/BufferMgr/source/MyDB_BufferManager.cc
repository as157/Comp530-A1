
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
    auto it = this->pageTable.find(key);
    
    if (it == this->pageTable.end()) {
        // not found in buffer therefore in DB table
        //check for availability in buffer
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
            
            //create pagehandle and return
            MyDB_PageHandle handle = make_shared<MyDB_PageHandleBase>(make_shared<MyDB_Page>(newPage));
            return handle;
        }
        
        //otherwise evict a page
        else{
            // first find node in list that is not pinned
            Node * evictNode = getNextNode();
            
            
            // remove from pageTable. Check dirty bit. If dirty writeback to db table. If not dirty just delete somehow. What happens to any pagehandles in the system that currently reference that page in buffer?
            // if yes create a new page handle and place in pageTable
        }
        
    } else {
        // found
        //create new PageHandle and return
        MyDB_PageHandle handle = make_shared<MyDB_PageHandleBase>(it->second);
        
        //******************update priority of node
        // remove node from list and reinsert at end of list
        Node * node = removeNode(it->second);
        insertNode(*node);
        
        return handle;
    }
    
    return nullptr;
}

// gets a temporary page that will no longer exist (1) after the buffer manager
// has been destroyed, or (2) there are no more references to it anywhere in the
// program.  Typically such a temporary page will be used as buffer memory.
// since it is just a temp page, it is not associated with any particular
// table

// get next available node
Node* MyDB_BufferManager :: getNextNode(){
    Node * currentNode = this->head;
    while(currentNode != NULL && currentNode->pageRef->pinned){
        currentNode = currentNode->next;
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
void MyDB_BufferManager :: insertNode(Node n){
    if(this->head == NULL){
        n.prev = NULL;
        n.next = NULL;
        this->head = &n;
    }
    else{
        n.next = NULL;
        n.prev = this->end;
        this->end->next = &n;
        this->end = &n;
    }
}

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
	
#endif


