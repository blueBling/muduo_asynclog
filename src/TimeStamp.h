// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "copyable.h"
#include "Types.h"




///
/// Time stamp in UTC, in microseconds resolution.
///
/// This class is immutable.
/// It's recommended to pass it by value, since it's passed in register on x64.
///
class Timestamp
{
public:
	///
	/// Constucts an invalid Timestamp.
	///
	Timestamp()
		: microSecondsSinceEpoch_(0)
	{
	}

	///
	/// Constucts a Timestamp at specific time
	///
	/// @param microSecondsSinceEpoch
	explicit Timestamp(int64_t microSecondsSinceEpochArg)
		: microSecondsSinceEpoch_(microSecondsSinceEpochArg)
	{
	}

	// 交换两个Timestamp 实质是交换两者microSecondsSinceEpoch_的值
	void swap(Timestamp& that)
	{
		std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
	}

	// default copy/assignment/dtor are Okay

	// 返回时间的字符串形式
	string toString() const;
	string toFormattedString(bool showMicroseconds = true) const;

	bool valid() const
	{
		return microSecondsSinceEpoch_ > 0;
	}

	// for internal usage.
	int64_t microSecondsSinceEpoch() const
	{
		return microSecondsSinceEpoch_;
	}

	//返回距离1970-1-1 00:00:00的秒数
	time_t secondsSinceEpoch() const
	{
		return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
	}

	///
	/// Get time of now.
	///
	static Timestamp now();
	static Timestamp invalid()
	{
		return Timestamp();
	}

	static Timestamp fromUnixTime(time_t t)
	{
		return fromUnixTime(t, 0);
	}

	static Timestamp fromUnixTime(time_t t, int microseconds)
	{
		return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
	}

	static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
	int64_t microSecondsSinceEpoch_; //记录距离1970-1-1 00:00:00 的微秒数
};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
	return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
	return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

///
/// Gets time difference of two timestamps, result in seconds.
///
/// @param high, low
/// @return (high-low) in seconds
/// @c double has 52-bit precision, enough for one-microsecond
/// resolution for next 100 years.
//获取时间间隔单位为秒
inline double timeDifference(Timestamp high, Timestamp low)
{
	int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
	return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

///
/// Add @c seconds to given timestamp.
///
/// @return timestamp+seconds as Timestamp
///
//从当前时间加上对应秒数
inline Timestamp addTime(Timestamp timestamp, double seconds)
{
	int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
	return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}



#endif  // TIMESTAMP_H
