// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "AsyncLogging.h"
#include "LogFile.h"
#include "Timestamp.h"

#include <stdio.h>

// https://blog.csdn.net/ma2595162349/article/details/102765004

AsyncLogging::AsyncLogging(const string& basename,
                           off_t rollSize,
                           int flushInterval)
	: flushInterval_(flushInterval),  // 日志落盘周期
	  running_(false),                // 日志线程运行标记
	  basename_(basename),            // 日志文件basename
	  rollSize_(rollSize),            // 预留的日志大小
	  thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"), // 执行该异步日志记录器的线程
	  latch_(1),
	  mutex_(),
	  cond_(mutex_),
	  currentBuffer_(new Buffer),  // 当前缓冲区
	  nextBuffer_(new Buffer),     // 预备缓冲区
	  buffers_()                   // 缓冲区队列
{
	currentBuffer_->bzero();
	nextBuffer_->bzero();
	buffers_.reserve(16);
}


// 向缓冲区追加日志信息，一般LOG_XX会通过Logger::setOutput进行输出控制来调用该append函数
void AsyncLogging::append(const char* logline, int len)
{
	MutexLockGuard lock(mutex_);
	// 如果当前buffer还有空间，就添加到当前日志
	if (currentBuffer_->avail() > len) {
		currentBuffer_->append(logline, len);
	} else {
        // 将使用完后的buffer添加到buffers_
		buffers_.push_back(std::move(currentBuffer_));

		if (nextBuffer_) { // 重新设置当前buffer
			currentBuffer_ = std::move(nextBuffer_);
		} else {
			currentBuffer_.reset(new Buffer); // 如果前端写入速度太快了，一下子把两块缓冲都用完了，那么只好分配一块新的buffer,作当前缓冲，这是极少发生的情况
		}
		 
		currentBuffer_->append(logline, len);

		// 通知日志线程，有数据可写 
		cond_.notify();
	}
}


// 线程调用的函数，主要用于周期性的flush数据到日志文件中
void AsyncLogging::threadFunc()
{
	assert(running_ == true);
	latch_.countDown();
	LogFile output(basename_, rollSize_, false);
	BufferPtr newBuffer1(new Buffer); // 这两个是后台线程的buffer
	BufferPtr newBuffer2(new Buffer);
	newBuffer1->bzero();
	newBuffer2->bzero();
	BufferVector buffersToWrite;      // 保存要写入的日志，用来和前台线程的buffers_进行swap
	buffersToWrite.reserve(16);
	while (running_) {
		assert(newBuffer1 && newBuffer1->length() == 0);
		assert(newBuffer2 && newBuffer2->length() == 0);
		assert(buffersToWrite.empty());

		{
			MutexLockGuard lock(mutex_); // 局部锁
			// 如果buffers_为空，那么表示没有数据需要写入文件，那么就等待指定的时间（注意这里没有用倒数计数器）
			if (buffers_.empty()) { // unusual usage!
				cond_.waitForSeconds(flushInterval_);   // 超时退出机制
			}

			// 无论cond是因何而醒来，无论currentBuffer_满不满，都要将currentBuffer_放到buffers_中
			// 调用移动构造，解决临时对象效率问题，同时  currentBuffer_之后被置空
			buffers_.push_back(std::move(currentBuffer_));
			// 用后台newBuffer1归还前台currentBuffer_
			currentBuffer_ = std::move(newBuffer1);
			 // 双队列，使用新的未使用的buffersToWrite交换buffers_，将buffers_中的数据在异步线程中写入LogFile中
			buffersToWrite.swap(buffers_);
			if (!nextBuffer_) {
				// 用后台newBuffer2归还前台nextBuffer_
				nextBuffer_ = std::move(newBuffer2);
			}
		}
		// 从这里是没有锁，数据落盘的时候不要加锁
		assert(!buffersToWrite.empty());

		// 如果将要写入文件的buffer列表中buffer的个数大于25，那么将多余数据删除	
		// 前端陷入死循环，拼命发送日志消息，超过后端的处理能力，会造成数据在内存中的堆积
		// 严重时引发性能问题(可用内存不足),或程序崩溃(分配内存失败)
		if (buffersToWrite.size() > 25) {
			char buf[256];
			snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
			         Timestamp::now().toFormattedString().c_str(),
			         buffersToWrite.size()-2);
			fputs(buf, stderr);
			output.append(buf, static_cast<int>(strlen(buf)));
			buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());   // 丢掉多余日志，以腾出内存，只保留2个buffer(默认4M)
		}

		// 将buffersToWrite的数据写入到日志中
		for (const auto& buffer : buffersToWrite) {
			// FIXME: use unbuffered stdio FILE ? or use ::writev ?
			output.append(buffer->data(), buffer->length());
		}
		output.flush();   // 保证数据落到磁盘了

		// 重新调整buffersToWrite的大小
		if (buffersToWrite.size() > 2) {
			// drop non-bzero-ed buffers, avoid trashing
			buffersToWrite.resize(2);
		}

		// 前台buffer是由newBuffer1 2 归还的。现在把buffersToWrite的buffer归还给后台buffer
		if (!newBuffer1) {
			assert(!buffersToWrite.empty());
			newBuffer1 = std::move(buffersToWrite.back());    // 复用，从buffersToWrite中弹出一个作为newBuffer1
			buffersToWrite.pop_back();
			newBuffer1->reset();
		}

		if (!newBuffer2) {
			assert(!buffersToWrite.empty());
			newBuffer2 = std::move(buffersToWrite.back());   // 复用，从buffersToWrite中弹出一个作为newBuffer2
			buffersToWrite.pop_back();
			newBuffer2->reset();
		}

		buffersToWrite.clear();

	}
	output.flush();
}

