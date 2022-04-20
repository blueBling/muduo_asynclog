//
//  test_timestamp.cc
//  test_timestamp
//
//  Created by blueBling on 22-3-31.
//  Copyright (c) 2022年blueBling. All rights reserved.
//


#include "Timestamp.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

int test_timestamp() {

	Timestamp time1(Timestamp::now());
	cout << time1.toString() << endl;
	cout << time1.toFormattedString(true) << endl;
	cout << time1.toFormattedString(false) << endl;

	// 创建1000000个Timestamp个对象所需要的时间
	const int kNumber = 1000*1000;
	std::vector<Timestamp> stamps;
	stamps.reserve(kNumber);

	for (int i = 0; i < kNumber; i++) {
		stamps.push_back(Timestamp::now());
	}

	cout << stamps.front().toFormattedString(false) << endl;
	cout << stamps.back().toFormattedString(false) << endl;
	cout << timeDifference(stamps.back(), stamps.front()) << endl;


	return 0;
}

int main() {

	test_timestamp();

	return 0;
}
