// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H

#include "Types.h"


namespace CurrentThread
{
// internal
extern __thread int t_cachedTid;            //线程id唯一标识
extern __thread char t_tidString[32];       //线程id唯一标识字符串形式
extern __thread int t_tidStringLength;      //线程id唯一标识字符串形式长度
extern __thread const char* t_threadName;   //当前线程名

//若当前tid为空，则初始化当前线程信息
void cacheTid();

//获取线程id唯一标识
inline int tid()
{
	if (__builtin_expect(t_cachedTid == 0, 0)) {
		cacheTid();
	}
	return t_cachedTid;
}

//获取tid字符串形式
inline const char* tidString() // for logging
{
	return t_tidString;
}

//获取tid字符串长度
inline int tidStringLength() // for logging
{
	return t_tidStringLength;
}

//获取线程名
inline const char* name()
{
	return t_threadName;
}

//判断是否是主进程
bool isMainThread();

//微妙级睡眠
void sleepUsec(int64_t usec);  // for testing

//获取堆栈信息，demangle决定是否要进行源程序标识符的转换
string stackTrace(bool demangle);
}  // namespace CurrentThread

#endif  // CURRENTTHREAD_H
