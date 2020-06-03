SHELL=/bin/bash

SRCPATH= ./src/
OBJPATH = ./bin/obj/
CC = gcc
CCFLAGS = -std=gnu90

OBJ = $(OBJPATH)wrapper.o $(OBJPATH)stats.o $(OBJPATH)config.o $(OBJPATH)commons.o
COMMONTARGETS = wrapper.o stats.o config.o commons.o
SOURCES = $(SRCPATH)wrapper.c $(SRCPATH)stats.c $(SRCPATH)config.c $(SRCPATH)commons.c
MAINDEPS = $(SRCPATH)wrapper.c $(SRCPATH)stats.c $(SRCPATH)config.c $(SRCPATH)commons.c $(SRCPATH)wrapper.h $(SRCPATH)stats.h $(SRCPATH)config.h $(SRCPATH)commons.h

.PHONY: build clean

build: bin main analyzer report

pipe:
	mkfifo /tmp/analyzerToReport.pipe
	mkfifo /tmp/mainToReport.pipe

bin:
	mkdir -p bin/obj

main: $(COMMONTARGETS) main.o
	$(CC) -o ./bin/main $(OBJ) $(OBJPATH)main.o $(CCFLAGS)

analyzer: $(COMMONTARGETS) reportConnector.o analyzer.o
	$(CC) -o ./bin/analyzer $(OBJPATH)analyzer.o $(OBJ) $(OBJPATH)reportConnector.o -pthread $(CCFLAGS)

report: $(COMMONTARGETS) report.o
	$(CC) -o ./bin/report $(OBJPATH)report.o $(OBJPATH)wrapper.o $(OBJPATH)stats.o $(OBJPATH)config.o $(OBJPATH)commons.o $(CCFLAGS)

main.o: $(SRCPATH)main.c
	$(CC) -c -o $(OBJPATH)main.o $(SRCPATH)main.c $(CCFLAGS)

report.o: $(SRCPATH)report.c
	$(CC) -c -o $(OBJPATH)report.o $(SRCPATH)report.c $(CCFLAGS)
	
analyzer.o: $(SRCPATH)analyzer.c 
	$(CC) -c -o $(OBJPATH)analyzer.o $(SRCPATH)analyzer.c $(CCFLAGS)
	
commons.o: $(SRCPATH)commons.c $(SRCPATH)commons.h
	$(CC) -c -o $(OBJPATH)commons.o $(SRCPATH)commons.c $(CCFLAGS)

stats.o: $(SRCPATH)stats.c $(SRCPATH)stats.h
	$(CC) -c -o $(OBJPATH)stats.o $(SRCPATH)stats.c $(CCFLAGS)

wrapper.o: $(SRCPATH)wrapper.c $(SRCPATH)commons.c $(SRCPATH)wrapper.h
	$(CC) -c -o $(OBJPATH)wrapper.o $(SRCPATH)wrapper.c $(CCFLAGS)

config.o: $(SRCPATH)config.c $(SRCPATH)config.h
	$(CC) -c -o $(OBJPATH)config.o $(SRCPATH)config.c $(CCFLAGS)
	
reportConnector.o: $(SRCPATH)reportConnector.c $(SRCPATH)reportConnector.h $(SRCPATH)config.h
	$(CC) -c -o $(OBJPATH)reportConnector.o $(SRCPATH)reportConnector.c $(CCFLAGS)

clean:
	rm -rf ./bin/
	rm -f /tmp/analyzerToReport.pipe
	rm -f /tmp/mainToReport.pipe

