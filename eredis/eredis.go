package eredis

/*
#include <stdlib.h>
#include "../redis/src/eredis.h"

// C Helpers
static char **allocStringArray(int size) {
	return calloc(sizeof(char *), size);
}

static size_t *allocLenArray(int size) {
	return calloc(sizeof(size_t), size);
}

static void setStringElement(char **strArray, size_t *sizeArray, char *s, size_t len, int idx) {
	strArray[idx] = s;
	sizeArray[idx] = len;
}

static void freeStringArray(char **array, int size) {
	int i;
	for (i = 0; i < size; i++) free(array[i]);
	free(array);
}

static void freeLenArray(size_t *array) {
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
	cargs := C.allocStringArray(C.int(len(args)))
	defer C.freeStringArray(cargs, C.int(len(args)))

	lengths := C.allocLenArray(C.int(len(args)))
	defer C.freeLenArray(lengths)

	for i, s := range args {
		C.setStringElement(cargs, lengths, C.CString(s), C.size_t(len(s)), C.int(i))
	}

	C.eredis_prepare_request(c.client, C.int(len(args)), cargs, lengths)
}

func (c *Client) Execute() error {
	if C.eredis_execute(c.client) != 0 {
		return errors.New("Execution failed")
	}

	return nil
}

func (c *Client) ReadReplyChunk() *string {
	var (
		len   C.int
		chunk *C.char
	)

	chunk = C.eredis_read_reply_chunk(c.client, &len)
	if chunk != nil {
		s := C.GoStringN(chunk, len)
		return &s
	} else {
		return nil
	}
}

func (c *Client) Release() {
	C.eredis_free_client(c.client)
}
