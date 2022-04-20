// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
// This is a public header file, it must only include public header files.

#ifndef FILEUTIL_H
#define FILEUTIL_H

#include "noncopyable.h"
#include "StringPiece.h"
#include <sys/types.h>  // for off_t


namespace FileUtil
{

// read small file < 64KB
class ReadSmallFile : noncopyable
{
public:
	ReadSmallFile(StringArg filename);
	~ReadSmallFile();

	// return errno
	template<typename String>
	int readToString(int maxSize,
	                 String* content,
	                 int64_t* fileSize,
	                 int64_t* modifyTime,
	                 int64_t* createTime);

	/// Read at maxium kBufferSize into buf_
	// return errno
	int readToBuffer(int* size);

	const char* buffer() const
	{
		return buf_;
	}

	static const int kBufferSize = 64*1024;

private:
	int fd_;                   //文件fd
	int err_;                  //errno
	char buf_[kBufferSize];    //文件内容
};

// read the file content, returns errno if error happens.
template<typename String>
int readFile(StringArg filename,
             int maxSize,
             String* content,
             int64_t* fileSize = NULL,
             int64_t* modifyTime = NULL,
             int64_t* createTime = NULL)
{
	ReadSmallFile file(filename);
	return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}

// not thread safe
class AppendFile : noncopyable
{
public:
	explicit AppendFile(StringArg filename);

	~AppendFile();

	void append(const char* logline, size_t len);

	void flush();

	off_t writtenBytes() const
	{
		return writtenBytes_;
	}

private:

	size_t write(const char* logline, size_t len);

	FILE* fp_;              //文件流句柄
	char buffer_[64*1024];  //文件流的缓冲区
	off_t writtenBytes_;    //写入字节数
};

}  // namespace FileUtil


#endif  // FILEUTIL_H

