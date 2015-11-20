CC = g++
CFLAGS = -Wall -std=c++11 -pthread -I/usr/local/include
LDFLAGS = -L/usr/local/lib

binaries = logger_bm.exe logger_bm_th.exe glog_bm.exe glog_bm_th.exe

all: $(binaries)

logger_bm.exe: logger_bm.cpp ../src/logger.c
	$(CC) -o $@ $^ $(CFLAGS) -I../src $(LDFLAGS)

logger_bm_th.exe: logger_bm_th.cpp ../src/logger.c
	$(CC) -o $@ $^ $(CFLAGS) -I../src $(LDFLAGS)

glog_bm.exe: glog_bm.cpp
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lglog

glog_bm_th.exe: glog_bm_th.cpp
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lglog

clean:
	rm logs/* $(binaries)
