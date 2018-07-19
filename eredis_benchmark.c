/*
 * A simple benchmark for the eredis library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

#include "redis/src/eredis.h"

static unsigned long get_mstime(void)
{
    struct timeval tv;
    long long t;

    gettimeofday(&tv, NULL);
    t = ((unsigned long) tv.tv_sec) * 1000;
    t += tv.tv_usec/1000;
    return t;
}

void run_benchmark(const char *name, int argc, const char **argv, unsigned long count, int reuse_req)
{
    eredis_client_t *c;
    unsigned long i;
    unsigned long stime, etime;
    int ret;

    c = eredis_create_client();
    assert(c != NULL);

    printf("%-30s : ...", name);
    stime = get_mstime();

    for (i = 0; i < count; i++) {
        if (!i || !reuse_req) {
            ret = eredis_prepare_request(c, argc, argv, NULL);
            assert(ret == 0);
        }

        ret = eredis_execute(c);
        assert(ret == 0);

        /* Drain response */
        int len;
        while (eredis_read_reply_chunk(c, &len) != NULL);
    }

    etime = get_mstime();

    printf("\b\b\b%lu ops/sec\n", count / (etime - stime) * 1000);
}


int main(int argc, char *argv[])
{
    int ret = eredis_init();
    assert(ret == 0);

    setvbuf(stdout, NULL, _IONBF, 0);

    static const char *cmd_set[] = {
        "SET", "mykey", "myvalue"
    };
    run_benchmark("SET command, reused query", 3, cmd_set, 1000000, 1);
    run_benchmark("SET command, regenerated query", 3, cmd_set, 1000000, 0);

    static const char *cmd_get[] = {
        "GET", "mykey"
    };
    run_benchmark("GET command, reused query", 3, cmd_get, 1000000, 1);
    run_benchmark("GET command, regenerated query", 3, cmd_get, 1000000, 0);
    
    exit(0);
}
