//
//  test_timezone.cc
//  test_timezone
//
//  Created by blueBling on 22-04-08.
//  Copyright (c) 2022年blueBling. All rights reserved.
//


#include "TimeZone.h"
#include "Date.h"
#include "TimeStamp.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;

void PrintTm(struct tm& T) {
	cout << T.tm_year << "-" << T.tm_mon << "-" << T.tm_mday << endl;
	cout << T.tm_hour << "-" << T.tm_min << "-" << T.tm_sec << endl;
	cout << T.tm_wday << endl;
}


// https://blog.csdn.net/qq_39898877/article/details/107884806
int test_timezone() {

	Timestamp timeStamp = Timestamp::now();
	struct tm T = TimeZone::toUtcTime(timeStamp.secondsSinceEpoch());//通过类调用静态成员函数
	PrintTm(T);//这个打印的月份要+1才是正确的月份时间，见Date.cc中的构造函数。
	Date dt(T);
	cout << dt.toIsoString() << endl;//显示正确的年月日
	//TimeZone timeZone(8, "China");//作者希望传入的是偏移的秒数，而不是偏移的时区数
	TimeZone timeZone(8*60*60, "China");
	struct	tm T2 = timeZone.toLocalTime(timeStamp.secondsSinceEpoch());
	PrintTm(T2);//这个能显示正确的时分秒

	return 0;
}

int main() {

	test_timezone();

	return 0;
}
