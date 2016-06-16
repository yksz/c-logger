What is this?
=============
This is a simple logging library for C/C++.

This library's main features:
* Lightweight - only 500-line source code
* C89 support
* Thread-safe
* 2 logging types:
  * Console logging
  * File logging rotated by file size
* Custom with a configuration file

Installation
============
```
./build.sh
cd build
make
sudo make install
```
or

```
build.bat
```
or

Copy files in src directory to your project

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
| 1        | 2.011s   | 1.822s   |
| 10       | 3.275s   | 1.981s   |

**Benchmark environment**
* Processor: Intel Core i3-4170 CPU @ 3.70GHz
* Memory: 4.0GB
* OS: Ubuntu 14.04 32bit

Log format
==========
```
level yy-MM-dd hh:mm:ss:uuuuuu threadid file:line: message
```

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

The output of the logger is the following:
```
I 15-11-10 00:32:43.771455 2854 filelogger.c:6: file logging
D 15-11-10 00:32:43.771564 2854 filelogger.c:7: format example: 123
```

License
=======
The MIT license
