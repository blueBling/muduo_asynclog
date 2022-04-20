// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "ProcessInfo.h"
#include "CurrentThread.h"
#include "FileUtil.h"

#include <algorithm>

#include <assert.h>
#include <dirent.h>
#include <pwd.h>
#include <stdio.h> // snprintf
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>


// /proc下文件参考 https://blog.spoock.com/2019/10/08/proc/

namespace detail
{
// 保存某个进程打开的文件数
__thread int t_numOpenedFiles = 0;

//struct dirent
//{
//   long d_ino;               /* inode number 索引节点号 */
//   off_t d_off;              /* offset to this dirent 在目录文件中的偏移 */
//   unsigned short d_reclen;  /* length of this d_name 文件名长 */
//   unsigned char d_type;     /* the type of d_name 文件类型 */
//   char d_name [NAME_MAX+1]; /* file name (null-terminated) 文件名，最长255字符 */
//}
// 获取进程打开文件数过滤条件
int fdDirFilter(const struct dirent* d)
{
	if (::isdigit(d->d_name[0])) {
		++t_numOpenedFiles;
	}
	return 0;
}

// 保存某个进程所有线程id
__thread std::vector<pid_t>* t_pids = NULL;
// 获取进程每一个线程的过滤条件
int taskDirFilter(const struct dirent* d)
{
	if (::isdigit(d->d_name[0])) {
		t_pids->push_back(atoi(d->d_name));
	}
	return 0;
}

// 按指定规则filter扫描dirpath
int scanDir(const char *dirpath, int (*filter)(const struct dirent *))
{
	struct dirent** namelist = NULL;
	int result = ::scandir(dirpath, &namelist, filter, alphasort);
	assert(namelist == NULL);
	return result;
}

Timestamp g_startTime = Timestamp::now();
// assume those won't change during the life time of a process.
int g_clockTicks = static_cast<int>(::sysconf(_SC_CLK_TCK)); //时钟频率
int g_pageSize = static_cast<int>(::sysconf(_SC_PAGE_SIZE)); //内存页大小
}  // namespace detail

using namespace detail;

// 获取进程识别码
pid_t ProcessInfo::pid()
{
	return ::getpid();
}

string ProcessInfo::pidString()
{
	char buf[32];
	snprintf(buf, sizeof buf, "%d", pid());
	return buf;
}

// 用来取得执行目前进程的用户识别码
uid_t ProcessInfo::uid()
{
	return ::getuid();
}

//struct passwd {   
//　　char *pw_name;     /*用户名*/                                                                 
//　　char *pw_passwd;   /*加密用户密码*/   
//　　int pw_uid;        /*用户id*/   
//　　int pw_gid;        /*组id*/   
//　　char *pw_gecos;    /*注释*/   
//　　char *pw_dir;      /*用户主目录*/   
//　　char *pw_shell;    /*shell目录*/
//};

// 获取执行目前进程的用户名
// 返回获取到的用户名，未获取到则返回unknownuser
string ProcessInfo::username()
{
	struct passwd pwd;
	struct passwd* result = NULL;
	char buf[8192];
	const char* name = "unknownuser";

	getpwuid_r(uid(), &pwd, buf, sizeof buf, &result);
	if (result) {
		name = pwd.pw_name;
	}
	return name;
}


// 获取最初执行程序时所用的ID，该ID是程序的所有者
uid_t ProcessInfo::euid()
{
	return ::geteuid();
}

// 获取进程启动时间
// 返回获取到的时间格式为Timestamp
Timestamp ProcessInfo::startTime()
{
	return g_startTime;
}

// 获取时钟频率(_SC_CLK_TCK)
int ProcessInfo::clockTicksPerSecond()
{
	return g_clockTicks;
}

// 获取内存页大小(_SC_PAGE_SIZE)
int ProcessInfo::pageSize()
{
	return g_pageSize;
}

bool ProcessInfo::isDebugBuild()
{
#ifdef NDEBUG
	return false;
#else
	return true;
#endif
}

// 获取本地主机的标准主机名
// 返回主机名，未获取到则返回unknownhost
string ProcessInfo::hostname()
{
	// HOST_NAME_MAX 64
	// _POSIX_HOST_NAME_MAX 255
	char buf[256];
	if (::gethostname(buf, sizeof buf) == 0) {
		buf[sizeof(buf)-1] = '\0';
		return buf;
	} else {
		return "unknownhost";
	}
}

string ProcessInfo::procname()
{
	return procname(procStat()).as_string();
}

// 获取当前进程名
StringPiece ProcessInfo::procname(const string& stat)
{
	StringPiece name;
	size_t lp = stat.find('(');
	size_t rp = stat.rfind(')');
	if (lp != string::npos && rp != string::npos && lp < rp) {
		name.set(stat.data()+lp+1, static_cast<int>(rp-lp-1));
	}
	return name;
}

// 获取当前进程状态信息，相较于/proc/self/stat更加可读
string ProcessInfo::procStatus()
{
	string result;
	FileUtil::readFile("/proc/self/status", 65536, &result);
	return result;
}

// 获取当前进程的状态信息.
string ProcessInfo::procStat()
{
	string result;
	FileUtil::readFile("/proc/self/stat", 65536, &result);
	return result;
}

// 获取当前线程的状态信息
string ProcessInfo::threadStat()
{
	char buf[64];
	snprintf(buf, sizeof buf, "/proc/self/task/%d/stat", CurrentThread::tid());
	string result;
	FileUtil::readFile(buf, 65536, &result);
	return result;
}

// 获取当前进程执行的二进制文件路径
string ProcessInfo::exePath()
{
	string result;
	char buf[1024];
	ssize_t n = ::readlink("/proc/self/exe", buf, sizeof buf);
	if (n > 0) {
		result.assign(buf, n);
	}
	return result;
}

// 获取当前进程打开的文件数
int ProcessInfo::openedFiles()
{
	t_numOpenedFiles = 0;
	scanDir("/proc/self/fd", fdDirFilter);
	return t_numOpenedFiles;
}

// 获取进程能打开的最大文件描述符个数
// 返回获取到的个数，若获取失败，返回当前进程打开的文件数
int ProcessInfo::maxOpenFiles()
{
	struct rlimit rl;
	if (::getrlimit(RLIMIT_NOFILE, &rl)) {
		return openedFiles();
	} else {
		return static_cast<int>(rl.rlim_cur);
	}
}

// 获取进程时间
ProcessInfo::CpuTime ProcessInfo::cpuTime()
{
	ProcessInfo::CpuTime t;
	struct tms tms;
	if (::times(&tms) >= 0) {
		const double hz = static_cast<double>(clockTicksPerSecond());
		t.userSeconds = static_cast<double>(tms.tms_utime) / hz;
		t.systemSeconds = static_cast<double>(tms.tms_stime) / hz;
	}
	return t;
}

// 获取当前进程组的线程数
int ProcessInfo::numThreads()
{
	int result = 0;
	string status = procStatus();
	size_t pos = status.find("Threads:");
	if (pos != string::npos) {
		result = ::atoi(status.c_str() + pos + 8);
	}
	return result;
}

// 获取当前进程每个线程id
std::vector<pid_t> ProcessInfo::threads()
{
	std::vector<pid_t> result;
	t_pids = &result;
	scanDir("/proc/self/task", taskDirFilter);
	t_pids = NULL;
	std::sort(result.begin(), result.end());
	return result;
}

