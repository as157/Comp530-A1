//
//  Node.hpp
//  A1
//
//  Created by Adrian Smart on 1/27/17.
//  Copyright © 2017 Adrian Smart. All rights reserved.
//

#ifndef Node_h
#define Node_h

#include <stdio.h>
#include "MyDB_Page.h"

#endif /* Node_hpp */

class Node{
public:
    Node * prev;
    Node * next;
    shared_ptr<MyDB_Page> pageRef;
    Node(shared_ptr<MyDB_Page> page);
};
