CC=gcc
CCFLAGS="-I. -std=gnu90"
DEPS=

#TODO: update make

%.o: %.c $(DEPS)
	$(CC) -c -o  $@ $< $(CCFLAGS)

%: %.o
	&(CC) -o $@ $< $(CCFLAGS)