/*
 * A simple test for the embedded redis library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "redis/src/eredis.h"
#include "acutest/include/acutest.h"

/* acutest may or may not fork per test, we handle both cases */
int redis_initialized = 0;

#define BEGIN_TEST() \
    if (!redis_initialized) { \
        int ret = eredis_init(); \
        TEST_CHECK(ret == 0); \
        redis_initialized = 1; \
    } \
    eredis_client_t *c = eredis_create_client(); \
    TEST_CHECK(c != NULL)

void test_set_command(void)
{
    BEGIN_TEST();

    int len;
    char *cmd[] = { "SET", "mykey", "myvalue" };

    eredis_prepare_request(c, 3, (const char **) &cmd, NULL);
    TEST_CHECK(eredis_execute(c) == 0);

    const char *reply = eredis_read_reply_chunk(c, &len);
    TEST_CHECK(len == 5);
    TEST_CHECK(!memcmp(reply, "+OK\r\n", 5));
}

void test_binary_args(void)
{
    BEGIN_TEST();

    int len;
    size_t cmd_lens[] = { 3, 5, 3 };
    char *cmd[] = { "SET", "mykey", "\0\0\0" };

    eredis_prepare_request(c, 3, (const char **) &cmd, cmd_lens);
    TEST_CHECK(eredis_execute(c) == 0);

    const char *reply = eredis_read_reply_chunk(c, &len);
    TEST_CHECK(len == 5);
    TEST_CHECK(!memcmp(reply, "+OK\r\n", 5));

    char *get_cmd[] = { "GET", "mykey" };
    eredis_prepare_request(c, 2, (const char **) &get_cmd, NULL);
    TEST_CHECK(eredis_execute(c) == 0);
    reply = eredis_read_reply_chunk(c, &len);
    TEST_CHECK(len == 9);
    TEST_CHECK(!memcmp(reply, "$3\r\n\0\0\0\r\n", 9));
}

void test_long_reply(void)
{
    BEGIN_TEST();

    int len;
    char buf[65536];
    char *set_cmd[] = { "SET", "mykey", buf };

    memset(buf, 'x', sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    eredis_prepare_request(c, 3, (const char **) &set_cmd, NULL);
    TEST_CHECK(eredis_execute(c) == 0);
    const char *reply = eredis_read_reply_chunk(c, &len);
    TEST_CHECK(len == 5);
    TEST_CHECK(!memcmp(reply, "+OK\r\n", 5));
    TEST_CHECK(eredis_read_reply_chunk(c, &len) == NULL);

    char *get_cmd[] = { "GET", "mykey" };

    eredis_prepare_request(c, 2, (const char **) &get_cmd, NULL);
    TEST_CHECK(eredis_execute(c) == 0);

    reply = eredis_read_reply_chunk(c, &len);
    TEST_CHECK(len == 8);
    TEST_CHECK(!memcmp(reply, "$65535\r\n", 8));

    reply = eredis_read_reply_chunk(c, &len);
    TEST_CHECK(len == 65535);
    TEST_CHECK(!memcmp(reply, buf, 65535));
}

void test_lua_reply(void)
{
    BEGIN_TEST();

    char *cmd[] = { "EVAL", "return {}", "0" };

    eredis_prepare_request(c, 3, (const char **) &cmd, NULL);
    TEST_CHECK(eredis_execute(c) == 0);

    int len;
    const char *reply = eredis_read_reply_chunk(c, &len);
    TEST_CHECK(len == 4);
    TEST_CHECK(!memcmp(reply, "*0\r\n", 4));
}


TEST_LIST = {
    { "test-set-command", test_set_command },
    { "test-binary-args", test_binary_args },
    { "test-long-reply", test_long_reply },
    { "test-lua-reply", test_lua_reply },
    { NULL, NULL }
};
