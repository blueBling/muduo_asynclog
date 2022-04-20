# 指定CMake版本
cmake_minimum_required(VERSION 3.5)
 
# 指定项目名称
project(test)
 
# 判断编译器类型,如果是gcc编译器,则在编译选项中加入c++11支持
if(CMAKE_COMPILER_IS_GNUCC)
    message("COMPILER IS GNUCC")
    add_definitions ( -std=c++11 )
endif(CMAKE_COMPILER_IS_GNUCC)
 
#使用Debug编译模式
SET(CMAKE_BUILD_TYPE "Debug")
 
# C++ 调试编译时使用的标志
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -ggdb3")
# C++ 发行编译时使用的标志
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(HOME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(SRC_DIR ${HOME_DIR}/src)
set(TESTS_DIR ${HOME_DIR}/tests)

message("HOME_DIR:${HOME_DIR}")
 
# 指定头文件目录
include_directories(${SRC_DIR})


# Atomic 依赖第三方库 pthread
find_package(Threads)
add_executable(test_atomic ${TESTS_DIR}/test_atomic.cc)
#target_link_libraries(test_atomic ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(test_atomic pthread)

# Date
file(GLOB_RECURSE DATE_SRCS ${SRC_DIR}/Date.cc)
add_executable(test_date ${DATE_SRCS} ${TESTS_DIR}/test_date.cc)


# Timestamp
file(GLOB_RECURSE TIMESTAMPS_SRCS ${SRC_DIR}/Timestamp.cc)
add_executable(test_timestamp ${TIMESTAMPS_SRCS} ${TESTS_DIR}/test_timestamp.cc)

# TimeZone
file(GLOB_RECURSE TIMEZONE_SRCS ${SRC_DIR}/TimeZone.cc)
add_executable(test_timezone ${DATE_SRCS} ${TIMESTAMPS_SRCS} ${TIMEZONE_SRCS} ${TESTS_DIR}/test_timezone.cc)

# ScopedPtr
add_executable(test_scopedptr ${TESTS_DIR}/test_scopedptr.cc)

# PtrVector
add_executable(test_ptrvector ${TESTS_DIR}/test_ptrvector.cc)

# AsyncLogging
aux_source_directory(${SRC_DIR} ASYNCLOG_SRCS)
add_executable(test_asynclog ${ASYNCLOG_SRCS} ${TESTS_DIR}/test_asynclog.cc)
target_link_libraries(test_asynclog pthread)