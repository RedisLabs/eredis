# Embedded Redis

This is a **toy attempt** to strip down a Redis server, load it as a shared object
and directly inject and execute commands bypassing all networking, etc.

## Building Embedded Library and C test

1. Make sure you `clone --recursive` to get submodules.
2. Run `make` to build the embedded Redis library and the sample C test.
3. Run `make tests` to run basic sanity tests on the C client.

## Go interface

The `eredis` dir contains a go package that wraps the C interface. To run a sample
test, use:

```
LD_LIBRARY_PATH=`pwd`/redis/src go test ./golang/eredis -v
```

Or just run:

```
make go-tests
```
