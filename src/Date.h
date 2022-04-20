// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef DATE_H
#define DATE_H

#include "copyable.h"
#include "Types.h"

struct tm;

///
/// Date in Gregorian calendar.
///
/// This class is immutable.
/// It's recommended to pass it by value, since it's passed in register on x64.
///

//儒略日（Julian Day）是在儒略周期内以连续的日数计算时间的计时法，主要是天文学家在使用。
//儒略日数（Julian Day Number，JDN）的计算是从格林威治标准时间的中午开始，包含一个整天的时间，起点的时间（0日）回溯至儒略历的公元前4713年1月1日中午12点（在格里历是公元前4714年11月24日），这个日期是三种多年周期的共同起点，且是历史上最接近现代的一个起点。
//例如，2000年1月1日的UT12:00是儒略日2,451,545。
class Date : public copyable
// public boost::less_than_comparable<Date>,
// public boost::equality_comparable<Date>
{
public:

	struct YearMonthDay {
		int year; // [1900..2500]
		int month;  // [1..12]
		int day;  // [1..31]
	};

	static const int kDaysPerWeek = 7;
	static const int kJulianDayOf1970_01_01;

	///
	/// Constucts an invalid Date.
	///
	Date()
		: julianDayNumber_(0)
	{}

	///
	/// Constucts a yyyy-mm-dd Date.
	///
	/// 1 <= month <= 12
	Date(int year, int month, int day);

	///
	/// Constucts a Date from Julian Day Number.
	///
	explicit Date(int julianDayNum)
		: julianDayNumber_(julianDayNum)
	{}

	///
	/// Constucts a Date from struct tm
	///
	explicit Date(const struct tm&);

	// default copy/assignment/dtor are Okay

	void swap(Date& that)
	{
		std::swap(julianDayNumber_, that.julianDayNumber_);
	}

	bool valid() const
	{
		return julianDayNumber_ > 0;
	}

	///
	/// Converts to yyyy-mm-dd format.
	///
	string toIsoString() const;

	struct YearMonthDay yearMonthDay() const;

	int year() const
	{
		return yearMonthDay().year;
	}

	int month() const
	{
		return yearMonthDay().month;
	}

	int day() const
	{
		return yearMonthDay().day;
	}

	// [0, 1, ..., 6] => [Sunday, Monday, ..., Saturday ]
	int weekDay() const
	{
		return (julianDayNumber_+1) % kDaysPerWeek;
	}

	int julianDayNumber() const
	{
		return julianDayNumber_;
	}

private:
	int julianDayNumber_;
};

inline bool operator<(Date x, Date y)
{
	return x.julianDayNumber() < y.julianDayNumber();
}

inline bool operator==(Date x, Date y)
{
	return x.julianDayNumber() == y.julianDayNumber();
}


#endif  // DATE_H
