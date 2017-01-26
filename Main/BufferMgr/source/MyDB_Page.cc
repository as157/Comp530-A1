//
//  MyDB_Page.cpp
//  A1
//
//  Created by Adrian Smart on 1/22/17.
//  Copyright © 2017 Adrian Smart. All rights reserved.
//

#include <stdio.h>
#include "MyDB_Page.h"

MyDB_Page :: MyDB_Page () {
}

MyDB_Page :: MyDB_Page(char* addr) {
    this->pageAddress = addr;
}

MyDB_Page :: ~MyDB_Page () {
}

