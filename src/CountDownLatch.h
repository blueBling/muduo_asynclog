// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include "Condition.h"
#include "Mutex.h"

//使一个线程等待其他线程各自执行完毕后再执行
class CountDownLatch : noncopyable
{
public:

	//初始值为线程的数量
	explicit CountDownLatch(int count);

	//调用wait方法的线程会被挂起，它会等待直到count_值为0才继续执行
	void wait();

	//当一个线程执行完毕后，计数器的值就-1，当计数器的值为0时，表示所有线程都执行完毕，然后在闭锁上等待的线程就可以恢复工作了
	void countDown();

	//获取计数
	int getCount() const;

private:
	mutable MutexLock mutex_;
	Condition condition_ GUARDED_BY(mutex_);
	int count_ GUARDED_BY(mutex_);
};
#endif  // COUNTDOWNLATCH_H
