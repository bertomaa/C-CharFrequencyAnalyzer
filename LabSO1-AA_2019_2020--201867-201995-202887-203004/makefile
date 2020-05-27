SHELL=/bin/bash

CC=gcc
CCFLAGS="-std=gnu90"
COMMONDEPS="./bin/wrapper.o ./bin/stats.o ./bin/config.o ./bin/commons.o"

#TODO: update make
#$(CC) -o  $@ $< $(CCFLAGS)

build: $$(DEPS)
	main
	analyzer
	report

main: main.o wrapper.o stats.o config.o commons.o
	$(CC) -o ./bin/main ./bin/main.o ./bin/wrapper.o ./bin/stats.o ./bin/config.o ./bin/commons.o $(CCFLAGS)

analyzer: analyzer.o $$(COMMONDEPS) reportConnector.o
	$(CC) -o ./bin/analyzer ./bin/analyzer.o $(COMMONDEPS) $(CCFLAGS)

report: report.o $$(COMMONDEPS)
	$(CC) -o ./bin/report ./bin/report.o $(COMMONDEPS) $(CCFLAGS)

main.o: ./src/main.c
	$(CC) -c -o ./bin/main.o ./src/main.c $(CCFLAGS)
	
analyzer.o: ./src/analyzer.c 
	$(CC) -c -o ./bin/analyzer.o ./src/analyzer.c $(CCFLAGS)
	
commons.o: ./src/commons.c ./src/commons.h
	$(CC) -c -o ./bin/commons.o ./src/commons.c $(CCFLAGS)

stats.o: ./src/stats.c
	$(CC) -c -o ./bin/stats.o ./src/stats.c $(CCFLAGS)

wrapper.o: ./src/wrapper.c ./src/commons.c
	$(CC) -c -o ./bin/wrapper.o ./src/wrapper.c $(CCFLAGS)

config.o: ./src/config.c
	$(CC) -c -o ./bin/config.o ./src/config.c $(CCFLAGS)
	
reportConnector.o: ./src/reportConnector.c
	$(CC) -c -o ./bin/reportConnector.o ./src/reportConnector.c $(CCFLAGS)

clean:
	rm -f ./bin/

.PHONY: build clean