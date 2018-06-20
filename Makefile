
CC = gcc
CFLAGS = -O2

all: eredis_test

clean:
	-rm -f eredis_test eredis_test.o

cleanall: clean
	-rm -f redis/src/liberedis.so
	$(MAKE) -C redis clean


redis/src/liberedis.so:
	$(MAKE) -C redis embedded

eredis_test: eredis_test.o redis/src/liberedis.so
	$(CC) eredis_test.o -o eredis_test -Lredis/src -leredis -Wl,-rpath `pwd`/redis/src -ljemalloc
