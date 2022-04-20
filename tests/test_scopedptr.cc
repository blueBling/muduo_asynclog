//
//  test_scopedptr.cc
//  test_scopedptr
//
//  Created by blueBling on 22-04-12.
//  Copyright (c) 2022年blueBling. All rights reserved.
//


#include "scoped_ptr.h"

#include "assert.h"

#include <iostream>
#include <memory>

using std::cout;
using std::endl;
using std::auto_ptr;

int test_scoped_ptr() {

	scoped_ptr<int> sp(new int(10));
	assert(sp == true);
	cout << *sp << endl;

	sp.reset(new int(20));
	cout << *sp << endl;

	scoped_ptr<int>sp2; 		   //另一个scoped_ptr
	//sp2 = sp;					   //赋值操作，无法同过编译！

	return 0;
}

int main() {

	test_scoped_ptr();

	return 0;
}
