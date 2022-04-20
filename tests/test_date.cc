//
//  test_date.cc
//  test_date
//
//  Created by blueBling on 22-3-31.
//  Copyright (c) 2022年blueBling. All rights reserved.
//


#include "Date.h"

#include <iostream>

using std::cout;
using std::endl;

int test_date() {

    struct tm* ttm;
    time_t tt = time(NULL);

    ttm = localtime(&tt);

    Date dt1(2022,3,31),dt2(*ttm);
    cout << dt1.julianDayNumber() << " " << dt2.julianDayNumber() << endl;
    cout << dt1.toIsoString() << endl;

	return 0;
}

int main() {

	test_date();

	return 0;
}
