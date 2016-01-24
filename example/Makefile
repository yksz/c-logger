CC = gcc
CFLAGS = -Wall -std=c89 -I../src

binaries = consolelogger.exe filelogger.exe

all: $(binaries)

consolelogger.exe: consolelogger.c ../src/logger.c
	$(CC) -o $@ $^ $(CFLAGS)

filelogger.exe: filelogger.c ../src/logger.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm logs/* $(binaries)
