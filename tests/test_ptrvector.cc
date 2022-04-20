//
//  test_ptrvector.cc
//  test_ptrvector
//
//  Created by blueBling on 22-04-13.
//  Copyright (c) 2022年blueBling. All rights reserved.
//


#include "ptr_vector.h"

#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::shared_ptr;
using std::unique_ptr;
using std::string;
using std::vector;

//https://www.cnblogs.com/my_life/articles/5452342.html
//https://www.bbsmax.com/A/kvJ3pKKw5g/
//https://www.codeproject.com/Articles/7351/ptr-vector-A-Container-For-Pointers

class TSomeData {
  private:
	int data;
  public:
	TSomeData(int d)
		: data(d) {
		// Empty
	}
};

const int TEST_ITERATIONS = 10000000;

typedef vector<unique_ptr<TSomeData>> TVectorOfUnique;
typedef vector<shared_ptr<TSomeData>> TVectorOfShared;
typedef myself::ptr_vector<TSomeData> TPtrVector;


int test_cmp2sharedptr() {

	clock_t start;
	clock_t end;

	start = ::clock();
	TVectorOfShared vectorOfShared;
	for (int i = 0; i < TEST_ITERATIONS; ++i) {
		// Test vector of shared_ptr
		shared_ptr<TSomeData> data(new TSomeData(i));
		vectorOfShared.push_back(data);
	}
	end = ::clock();

	cout << "Vector of shared:\n  Time executed: " 
	     << static_cast<uint32_t>((end - start) / (CLOCKS_PER_SEC/1000)) 
	     << endl;

	// ********************************************************************* //

	start = ::clock();
	TVectorOfUnique vectorOfUnique;
	for (int i = 0; i < TEST_ITERATIONS; ++i) {
	  unique_ptr<TSomeData> data(new TSomeData(i));
	  vectorOfUnique.push_back(std::move(data));
	}
	end = ::clock();

	cout << "Vector of unique:\n  Time executed: "
	     << static_cast<uint32_t>((end - start) / (CLOCKS_PER_SEC/1000)) 
	     << endl;

	// ********************************************************************* //

	start = ::clock();
	TPtrVector ptrVector;
	for (int i = 0; i < TEST_ITERATIONS; ++i) {
		// Test ptr_vector
		TSomeData* data = new TSomeData(i);
		ptrVector.push_back(data);
	}
	end = ::clock();

	cout << "PtrVector:\n  Time executed: "
	     << static_cast<uint32_t>((end - start) / (CLOCKS_PER_SEC/1000)) 
	     << endl;

	return 0;
}

int test_ptr_vector() {

	myself::ptr_vector<string> vec;
	vec.push_back (new string ("Hello, "));
	vec.push_back (new string ("world! "));

	//cout << vec[0]		 << vec.at(1)
	//     << *vec.begin() << vec.begin()[1]
	//    << vec.front()  << vec.back()
	//     << endl;

	cout << *vec[0] << endl;


	return 0;
}


int main() {

	test_cmp2sharedptr();

	test_ptr_vector();

	return 0;
}
