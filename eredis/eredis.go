package eredis

/*
#include <stdlib.h>
#include "../redis/src/eredis.h"

// C Helpers
static char **allocArray(int size) {
	return calloc(sizeof(char *), size);
}

static void setElement(char **a, char *s, int idx) {
	a[idx] = s;
}

static void freeArray(char **array, int size) {
	int i;
	for (i = 0; i < size; i++) free(array[i]);
	free(array);
}

#cgo LDFLAGS: -L../redis/src -leredis
*/
import "C"

import (
	"errors"
)

type Client struct {
	client *_Ctype_struct_eredis_client
}

var initialized bool = false

func Init() error {
	if initialized {
		return errors.New("Already initialized")
	}

	if C.eredis_init() != 0 {
		return errors.New("Redis initialization failed")
	}

	initialized = true
	return nil
}

func CreateClient() *Client {
	return &Client{
		client: C.eredis_create_client(),
	}
}

func (c *Client) PrepareRequest(args []string) {
	cargs := C.allocArray(C.int(len(args)))
	defer C.freeArray(cargs, C.int(len(args)))

	for i, s := range args {
		C.setElement(cargs, C.CString(s), C.int(i))
	}

	C.eredis_prepare_request(c.client, C.int(len(args)), cargs)
}

func (c *Client) Execute() string {
	var (
		replyLen C.int
		reply    *C.char
	)

	reply = C.eredis_execute(c.client, &replyLen)
	return C.GoStringN(reply, replyLen)
}

func (c *Client) Release() {
	// Not implemented yet
}
