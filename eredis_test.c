/*
 * A simple test for the embedded redis library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "redis/src/eredis.h"

int main(int argc, char *argv[])
{
    int ret;

    fprintf(stderr, "Initializing Embedded Redis...\n");
    if ((ret = eredis_init()) < 0) {
        fprintf(stderr, "Failure, error code = %d\n", ret);
        exit(1);
    }

    eredis_client_t *c = eredis_create_client();

    struct timeval start, end;
    gettimeofday(&start, NULL);

    int len;
    char *cmd[] = {
        "SET", "mykey", "myvalue"
    };

    eredis_prepare_request(c, 3, (const char **) &cmd);
    const char *reply = eredis_execute(c, &len);

    fprintf(stderr, "Reply: [%.*s]\n", len, reply);

    exit(0);
}

