//
//  test_asynclog.cc
//  test_asynclog
//
//  Created by blueBling on 22-3-31.
//  Copyright (c) 2022年blueBling. All rights reserved.
//


#include "AsyncLogging.h"
#include "Logging.h"
#include "TimeStamp.h"

#include <iostream>

using std::cout;
using std::endl;

#define LOG_NUM 5000000 // 总共的写入日志行数

static AsyncLogging *g_asyncLog = NULL;

static void asyncOutput(const char *msg, int len)
{
	g_asyncLog->append(msg, len);
}

int test_asynclog() {

	off_t kRollSize = 1 * 1000 * 1000;	  // 只设置1M

	char logfile[128] = "async_log_";
	AsyncLogging log(logfile, kRollSize, 1);
	Logger::setOutput(asyncOutput);
	g_asyncLog = &log;
	log.start();        // 启动日志写入线程


	Timestamp begin_time = Timestamp::now();
	cout << "begin time: " << begin_time.toFormattedString(false) << endl;
	
	for (int i = 0; i < LOG_NUM; i++) {
		LOG_INFO << "NO." << i << " Log Info Message!";
	}

	log.stop();
	
	Timestamp end_time = Timestamp::now();
	cout << "end time: " << end_time.toFormattedString(false) << endl;
	

	double consume_time = timeDifference(end_time, begin_time);

	cout << "need " << consume_time << "(s)  ops:" <<  (LOG_NUM / (consume_time)) << "/s" << endl;


	return 0;
}

int main() {

	test_asynclog();

	return 0;
}
