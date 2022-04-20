// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include "Condition.h"
#include "Mutex.h"

#include <deque>
#include <assert.h>


template<typename T>
class BlockingQueue : noncopyable
{
public:
	BlockingQueue()
		: mutex_(),
		  notEmpty_(mutex_),
		  queue_()
	{
	}

	//非常量可以初始化一个底层const，反过来却不行
	//可以接收任何能转换成类型T的任何对象
	void put(const T& x)
	{
		MutexLockGuard lock(mutex_);
		queue_.push_back(x);
		notEmpty_.notify(); // wait morphing saves us
		// http://www.domaigne.com/blog/computing/condvars-signal-with-mutex-locked-or-not/
	}

	//只可以传递非const右值，由于精确匹配规则，传递非const右值时，会调用该版本，尽管上面版本也可以接受。
	//一般来说，我们不需要为函数操作定义接受一个const T&&或是一个普通的T&参数的版本。
	//当我们希望从实参“窃取‘数据时，通常传递一个右值引用。为了达到这一目的，实参不能使const的。
	//类似的，从一个对象进行拷贝的操作，不应该改变该对象，因此，通常不需要定义一个接受一个普通的T&参数的版本
	void put(T&& x)
	{
		MutexLockGuard lock(mutex_);
		queue_.push_back(std::move(x));
		notEmpty_.notify();
	}

	T take()
	{
		MutexLockGuard lock(mutex_);
		// always use a while-loop, due to spurious wakeup
		while (queue_.empty()) {
			notEmpty_.wait();
		}
		assert(!queue_.empty());
		T front(std::move(queue_.front()));
		queue_.pop_front();
		return front;
	}

	size_t size() const
	{
		MutexLockGuard lock(mutex_);
		return queue_.size();
	}

private:
	mutable MutexLock mutex_;
	Condition         notEmpty_ GUARDED_BY(mutex_);
	std::deque<T>     queue_ GUARDED_BY(mutex_);
};


#endif  // BLOCKINGQUEUE_H
