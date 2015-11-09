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
```

Platforms
=========
* Windows (Visual Studio 2008+)
* Mac OS X
* Linux

Example
=======
#### Console logging
```c
logger_initConsoleLogger(stderr);
logger_setLogLevel(LogLevel_DEBUG);
LOG_INFO("console logging");
```

#### File logging
```c
logger_initFileLogger("logs/log.txt", 1024 * 1024, 5);
logger_setLogLevel(LogLevel_DEBUG);
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
