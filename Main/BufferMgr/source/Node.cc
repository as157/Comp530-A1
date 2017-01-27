//
//  Node.cpp
//  A1
//
//  Created by Adrian Smart on 1/27/17.
//  Copyright © 2017 Adrian Smart. All rights reserved.
//

#include "Node.h"

Node::Node(shared_ptr<MyDB_Page> page){
    this->pageRef = page;
}
