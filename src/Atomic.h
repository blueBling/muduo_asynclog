// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef ATOMIC_H
#define ATOMIC_H

#include "noncopyable.h"

#include <stdint.h>


namespace detail
{
template<typename T>
class AtomicIntegerT : noncopyable
{
public:
	AtomicIntegerT()
		: value_(0)
	{
	}

	// uncomment if you need copying and assignment
	//
	// AtomicIntegerT(const AtomicIntegerT& that)
	//   : value_(that.get())
	// {}
	//
	// AtomicIntegerT& operator=(const AtomicIntegerT& that)
	// {
	//   getAndSet(that.get());
	//   return *this;
	// }

	//获取当前计数值
	T get()
	{
		// in gcc >= 4.7: __atomic_load_n(&value_, __ATOMIC_SEQ_CST)
		//type __sync_val_compare_and_swap (type *ptr, type oldval type newval, ...)
		//比较*ptr与oldval的值，如果两者相等，则将newval更新到*ptr并返回操作之前*ptr的值
		return __sync_val_compare_and_swap(&value_, 0, 0);
	}

	//将当前计数加x,并返回加之前的计数值
	T getAndAdd(T x)
	{
		// in gcc >= 4.7: __atomic_fetch_add(&value_, x, __ATOMIC_SEQ_CST)
		//将&value_指向的内存加上x值, 并且返回value_原来的值
		return __sync_fetch_and_add(&value_, x);
	}

	//将当前计数加x,并返回加之后的计数值
	T addAndGet(T x)
	{
		return getAndAdd(x) + x;
	}

	//将当前计数加1,并返回加之后的计数值
	T incrementAndGet()
	{
		return addAndGet(1);
	}

	//将当前计数减1,并返回加之后的计数值
	T decrementAndGet()
	{
		return addAndGet(-1);
	}

	//将当前计数加x
	void add(T x)
	{
		getAndAdd(x);
	}

	//将当前计数加1
	void increment()
	{
		incrementAndGet();
	}

	//将当前计数减1
	void decrement()
	{
		decrementAndGet();
	}

	//设置当前计数为newValue，并返回之前的计数
	T getAndSet(T newValue)
	{
		// in gcc >= 4.7: __atomic_exchange_n(&value_, newValue, __ATOMIC_SEQ_CST)
		//将newValue写入value_，对value_加锁，并返回操作之前value_的值。即，try spinlock语义
		return __sync_lock_test_and_set(&value_, newValue);
	}

private:
	volatile T value_;
};
}  // namespace detail

typedef detail::AtomicIntegerT<int32_t> AtomicInt32; //32位int型原子计数
typedef detail::AtomicIntegerT<int64_t> AtomicInt64; //64位int型原子计数

#endif  // ATOMIC_H
