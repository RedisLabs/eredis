
REDIS_SHA = 00613bed06af890b9f09befc71de3f2aedabbacb
REDIS_URL = https://codeload.github.com/antirez/redis/tar.gz/$(REDIS_SHA)

CC = gcc
CFLAGS = -O2 -g

all: eredis_test eredis_benchmark

clean:
	-rm -f eredis_test eredis_test.o

cleanall: clean
	-rm -rf redis

redis/src/liberedis.so: redis/.patched
	$(MAKE) -C redis embedded

redis/.patched:
	mkdir -p redis && \
	cd redis && \
	curl --silent $(REDIS_URL) | gunzip -c | tar xf - --strip 1 && \
	patch -p1 < ../eredis.diff && \
	touch .patched

eredis_test: redis/src/liberedis.so eredis_test.o
	$(CC) eredis_test.o -o eredis_test -Lredis/src -leredis -Wl,-rpath `pwd`/redis/src -ljemalloc

eredis_benchmark: redis/src/liberedis.so eredis_benchmark.o
	$(CC) eredis_benchmark.o -o eredis_benchmark -Lredis/src -leredis -Wl,-rpath `pwd`/redis/src -ljemalloc

tests: eredis_test
	./eredis_test

go-tests: redis/src/liberedis.so
	LD_LIBRARY_PATH=`pwd`/redis/src go test -v ./golang/eredis
