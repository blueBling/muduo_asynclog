// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef LOGGING_H
#define LOGGING_H

#include "LogStream.h"
#include "Timestamp.h"


class TimeZone;

// https://www.icode9.com/content-4-399239.html

class Logger
{
public:
	enum LogLevel {
		TRACE,
		DEBUG,   // 上个和这个是调试的时候用,记录详细的程序运行过程,各变量的变化
		INFO,    // 程序运行信息,上线后一般用这个等级
		WARN,    // 警告
		ERROR,   // 错误信息
		FATAL,   // 严重错误，会立即将日志信息落盘并abort()
		NUM_LOG_LEVELS,
	};

	// compile time calculation of basename of source file
	class SourceFile
	{
	public:
		template<int N>
		SourceFile(const char (&arr)[N])
			: data_(arr),
			  size_(N-1)
		{
			const char* slash = strrchr(data_, '/'); // builtin function
			if (slash) {
				data_ = slash + 1;
				size_ -= static_cast<int>(data_ - arr);
			}
		}

		explicit SourceFile(const char* filename)
			: data_(filename)
		{
			const char* slash = strrchr(filename, '/');
			if (slash) {
				data_ = slash + 1;
			}
			size_ = static_cast<int>(strlen(data_));
		}

		const char* data_; // 存储文件名
		int size_;         // 文件名大小
	};

	// 4种构造函数，根据参数将额外信息加到输出缓冲区
	Logger(SourceFile file, int line);
	Logger(SourceFile file, int line, LogLevel level);
	Logger(SourceFile file, int line, LogLevel level, const char* func);
	Logger(SourceFile file, int line, bool toAbort);
	~Logger(); // 把缓冲区中的内容取出来,用g_output输出到特定文件,默认为stdout

	LogStream& stream()
	{
		return impl_.stream_;
	}

	static LogLevel logLevel();
	static void setLogLevel(LogLevel level);

	typedef void (*OutputFunc)(const char* msg, int len); // 输出的控制函数,默认输出到stdout
	typedef void (*FlushFunc)(); // 刷新的回调函数,默认刷新标准输出
	static void setOutput(OutputFunc);
	static void setFlush(FlushFunc);
	static void setTimeZone(const TimeZone& tz);

private:

	// 将日志事件(时间 日志级别 文件名 行号等)信息加到输出缓冲区
	class Impl
	{
	public:
		typedef Logger::LogLevel LogLevel;
		Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
		void formatTime(); // 格式化时间到输出缓冲区
		void finish();        // 结束后,给输出缓冲区加上basename_和行数

		Timestamp time_;
		LogStream stream_;
		LogLevel level_;
		int line_;
		SourceFile basename_;
	};

	Impl impl_;

};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel()
{
	return g_logLevel;
}

//
// CAUTION: do not write:
//
// if (good)
//   LOG_INFO << "Good news";
// else
//   LOG_WARN << "Bad news";
//
// this expends to
//
// if (good)
//   if (logging_INFO)
//     logInfoStream << "Good news";
//   else
//     logWarnStream << "Bad news";
//

// 如果level<=当前级别，用一个匿名Logger对象,调用stream(),返回一个LogStream类型的引用,这个类重载了<<运算符,然后把信息输入到缓冲区
#define LOG_TRACE if (Logger::logLevel() <= Logger::TRACE) \
  Logger(__FILE__, __LINE__, Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (Logger::logLevel() <= Logger::DEBUG) \
  Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO if (Logger::logLevel() <= Logger::INFO) \
  Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()
#define LOG_SYSERR Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL Logger(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int savedErrno);

// Taken from glog/logging.h
//
// Check that the input is non NULL.  This very useful in constructor
// initializer lists.

#define CHECK_NOTNULL(val) \
  ::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

// A small helper for CHECK_NOTNULL().
template <typename T>
T* CheckNotNull(Logger::SourceFile file, int line, const char *names, T* ptr)
{
	if (ptr == NULL) {
		Logger(file, line, Logger::FATAL).stream() << names;
	}
	return ptr;
}



#endif  // LOGGING_H
