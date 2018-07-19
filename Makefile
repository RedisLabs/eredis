
REDIS_SHA = 00613bed06af890b9f09befc71de3f2aedabbacb
REDIS_URL = https://codeload.github.com/antirez/redis/tar.gz/$(REDIS_SHA)
ifeq ($(shell uname -s), Darwin)
DYLIB_SETUP=DYLD_LIBRARY_PATH=`pwd`/redis/src
else
DYLIB_SETUP=LD_LIBRARY_PATH=`pwd`/redis/src
EXTRA_LDFLAGS = -Wl,-rpath `pwd`/redis/src
endif

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
	$(CC) eredis_test.o -o eredis_test -Lredis/src -leredis $(EXTRA_LDFLAGS) $(EXTRA_LIBS)

eredis_benchmark: redis/src/liberedis.so eredis_benchmark.o
	$(CC) eredis_benchmark.o -o eredis_benchmark -Lredis/src -leredis $(EXTRA_LDFLAGS) $(EXTRA_LIBS)

tests: eredis_test
	$(DYLIB_SETUP) ./eredis_test

go-tests: redis/src/liberedis.so
	$(DYLIB_SETUP) go test -v ./golang/eredis

benchmark: eredis_benchmark
	$(DYLIB_SETUP) ./eredis_benchmark
