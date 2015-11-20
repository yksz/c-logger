What is this?
=============
This is a simple logging library for C/C++.

This library's main features:
* Lightweight - only 300-line source code
* C89 support
* Thread-safe
* 2 logging types:
  * Console logging
  * File logging rotated by file size

Installation
============
```shell
mkdir build
cd build
cmake ..
make
sudo make install
```

Platform
========
* Windows (Visual Studio 2008+)
* Mac OS X (clang 3.6+)
* Linux (gcc 4.8.4+)

Note: Operations are confirmed on the above platforms

Benchmark
=========
This following table comparing other libraries is benchmark results to log 1,000,000 times.

| threads  | c-logger | glog     |
|:---------|---------:|---------:|
| 1        | 1.687s   | 1.822s   |
| 10       | 3.022s   | 1.981s   |

**Benchmark environment**
* Processor: Intel Core i3-4170 CPU @ 3.70GHz
* Memory: 4.0GB
* OS: Ubuntu 14.04 32bit

Example
=======
#### Console logging
```c
logger_initConsoleLogger(stderr);
logger_setLevel(LogLevel_DEBUG);
LOG_INFO("console logging");
```

#### File logging
```c
logger_initFileLogger("logs/log.txt", 1024 * 1024, 5);
logger_setLevel(LogLevel_DEBUG);
LOG_INFO("file logging");
LOG_DEBUG("format example: %d%c%s", 1, '2', "3");
```

The output of the logger is as following:

```
2015-11-10 00:00:01 INFO  filelogger.c:6:main: file logging
2015-11-10 00:00:01 DEBUG filelogger.c:7:main: format example: 123
```

License
=======
The MIT license
