//
//  test_atomic.cc
//  test_atomic
//
//  Created by blueBling on 22-3-7.
//  Copyright (c) 2022年blueBling. All rights reserved.
//


#include "Atomic.h"

#include <iostream>
#include <thread>

using std::cout;
using std::endl;
using std::thread;
//using std::this_thread;

uint32_t g_cnt = 0;
AtomicInt32 g_atomic_cnt;

void foo() 
{
	for(int i = 0; i < 10000000; i++) {
		g_atomic_cnt.increment();
		g_cnt++;
	}
}

void bar()
{
  	for(int i = 0; i < 10000000; i++) {
		g_atomic_cnt.increment();
		g_cnt++;
	}
}


int test_atomic() {

    cout << "主线程的线程ID: " << std::this_thread::get_id() << endl;

	thread first(foo);	 // spawn new thread that calls foo()
	thread second(bar);  // spawn new thread that calls bar(0)
	
	cout << "main, foo and bar now execute concurrently...\n";

	cout << "线程first的线程ID: " << first.get_id() << endl;
    cout << "线程second的线程ID: " << second.get_id() << endl;

	// synchronize threads:
	first.join();				 // pauses until first finishes
	second.join();				 // pauses until second finishes
	
	cout << "foo and bar completed." << endl;
	cout << "g_atomic_cnt is:" << g_atomic_cnt.get() << ", g_cnt is:" << g_cnt << endl;

	return 0;
}


int main() {

	test_atomic();

	return 0;
}
