SHELL=/bin/bash

SRCPATH= ./src/
OBJPATH = ./bin/obj/
BINPATH = ./bin/
CC = gcc
CCFLAGS = -std=gnu90 -Wall -ggdb3

OBJ = $(OBJPATH)wrapper.o $(OBJPATH)stats.o $(OBJPATH)config.o $(OBJPATH)commons.o $(OBJPATH)forkHandler.o
COMMONTARGETS = $(OBJPATH)wrapper.o $(OBJPATH)stats.o $(OBJPATH)config.o $(OBJPATH)commons.o $(OBJPATH)forkHandler.o



.PHONY: build clean pipe 

build: bin/obj  $(BINPATH)main $(BINPATH)analyzer $(BINPATH)report

pipe:
	test -e /tmp/analyzerToReport.pipe && mkfifo /tmp/analyzerToReport.pipe;

bin/obj:
	mkdir -p bin/obj

$(BINPATH)main: $(COMMONTARGETS) $(OBJPATH)main.o 
	$(CC) -o ./bin/main $(OBJ) $(OBJPATH)main.o $(CCFLAGS)

$(BINPATH)analyzer: $(COMMONTARGETS) $(OBJPATH)reportConnector.o $(OBJPATH)analyzer.o
	$(CC) -o ./bin/analyzer $(OBJPATH)analyzer.o $(OBJ) $(OBJPATH)reportConnector.o -pthread $(CCFLAGS)

$(BINPATH)report: $(COMMONTARGETS) $(OBJPATH)report.o
	$(CC) -o ./bin/report $(OBJPATH)report.o $(OBJ) $(CCFLAGS)

$(OBJPATH)main.o: $(SRCPATH)main.c
	$(CC) -c -o $(OBJPATH)main.o $(SRCPATH)main.c $(CCFLAGS)

$(OBJPATH)report.o: $(SRCPATH)report.c
	$(CC) -c -o $(OBJPATH)report.o $(SRCPATH)report.c $(CCFLAGS)
	
$(OBJPATH)analyzer.o: $(SRCPATH)analyzer.c 
	$(CC) -c -o $(OBJPATH)analyzer.o $(SRCPATH)analyzer.c $(CCFLAGS)
	
$(OBJPATH)commons.o: $(SRCPATH)commons.c $(SRCPATH)commons.h
	$(CC) -c -o $(OBJPATH)commons.o $(SRCPATH)commons.c $(CCFLAGS)

$(OBJPATH)stats.o: $(SRCPATH)stats.c $(SRCPATH)stats.h
	$(CC) -c -o $(OBJPATH)stats.o $(SRCPATH)stats.c $(CCFLAGS)

$(OBJPATH)wrapper.o: $(SRCPATH)wrapper.c $(SRCPATH)commons.c $(SRCPATH)wrapper.h
	$(CC) -c -o $(OBJPATH)wrapper.o $(SRCPATH)wrapper.c $(CCFLAGS)

$(OBJPATH)config.o: $(SRCPATH)config.c $(SRCPATH)config.h
	$(CC) -c -o $(OBJPATH)config.o $(SRCPATH)config.c $(CCFLAGS)
	
$(OBJPATH)reportConnector.o: $(SRCPATH)reportConnector.c $(SRCPATH)reportConnector.h $(SRCPATH)config.h
	$(CC) -c -o $(OBJPATH)reportConnector.o $(SRCPATH)reportConnector.c $(CCFLAGS)

$(OBJPATH)forkHandler.o: $(SRCPATH)forkHandler.c $(SRCPATH)forkHandler.h $(SRCPATH)commons.h $(SRCPATH)wrapper.h
	$(CC) -c -o $(OBJPATH)forkHandler.o $(SRCPATH)forkHandler.c $(CCFLAGS)

clean:
	rm -rf ./bin/
	rm -f /tmp/analyzerToReport.pipe
	rm -f /tmp/mainToReport.pipe

