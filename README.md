# muduo_asynclog
muduo异步日志库


#### 1. 简介
将muduo异步日志单独拎出来，同时对使用到的关键模块进行解析，主要目的用于学习。

#### 2. 目录说明
tests: 存放测试代码源文件
tests/build: CMakeLists.txt存放路径，该目录用于防止cmake生成文件和测试源文件混在一起
src: 异步日志源文件

#### 3. 测试代码编译运行
在tests/build目录下执行
```
cmake .
make
```
此时会在当前目录下生成test_xxx等可执行文件，执行./test_xxx即可运行

#### 4. 各个模块解析
参考： https://blog.csdn.net/aixiaoql/category_11671324.html
