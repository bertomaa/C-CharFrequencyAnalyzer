SHELL=/bin/bash

CC=gcc
CCFLAGS="-I. -std=gnu90"
DEPS="./src/analyzer.c ./src/main.c ./src/report.c"

#TODO: update make


build: $$(DEPS)
	$(CC) -c -o  $@ $< $(CCFLAGS)

clean:
	rm -f ./bin/

.PHONY: build clean