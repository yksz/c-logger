CC = gcc
CFLAGS = -Wall -std=c89 -I../src

binaries = conf.exe consolelogger.exe filelogger.exe

all: $(binaries)

conf.exe: conf.c ../src/logger.c ../src/loggerconf.c
	$(CC) -o $@ $^ $(CFLAGS)

consolelogger.exe: consolelogger.c ../src/logger.c
	$(CC) -o $@ $^ $(CFLAGS)

filelogger.exe: filelogger.c ../src/logger.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm logs/* $(binaries)
