package eredis

import (
	"strings"
	"testing"
)

func assertEqual(t *testing.T, a interface{}, b interface{}) {
	if a != b {
		t.Fatalf("%s != %s", a, b)
	}
}

func TestEmbeddedRedis(t *testing.T) {
	err := Init()
	assertEqual(t, err, nil)

	c := CreateClient()
	defer c.Release()

	t.Log("Initialized embedded redis")

	// Basic SET
	t.Log("Doing basic SET command")
	c.PrepareRequest([]string{"SET", "mykey", "myval"})
	err = c.Execute()
	assertEqual(t, err, nil)

	reply := c.ReadReplyChunk()
	assertEqual(t, *reply, "+OK\r\n")
	reply = c.ReadReplyChunk()
	if reply != nil {
		t.Fatal("Unexpected chunk")
	}

	// SET/GET with binary (null) data
	t.Log("Doing SET/GET with non-null-safe binary data")
	c.PrepareRequest([]string{"SET", "mykey", "\000\000\000"})
	err = c.Execute()
	assertEqual(t, err, nil)

	c.PrepareRequest([]string{"GET", "mykey"})
	err = c.Execute()
	assertEqual(t, err, nil)

	reply = c.ReadReplyChunk()
	assertEqual(t, *reply, "$3\r\n\000\000\000\r\n")
	if c.ReadReplyChunk() != nil {
		t.Fatal("Unexpected chunk")
	}

	// SET/GET with long data
	t.Log("Doing GET with long chunked reply")
	c.PrepareRequest([]string{"SET", "mykey", strings.Repeat("x", 100000)})
	err = c.Execute()
	assertEqual(t, err, nil)

	c.PrepareRequest([]string{"GET", "mykey"})
	err = c.Execute()
	assertEqual(t, err, nil)

	reply = c.ReadReplyChunk()
	assertEqual(t, *reply, "$100000\r\n")
	reply = c.ReadReplyChunk()
	assertEqual(t, *reply, strings.Repeat("x", 100000))
	reply = c.ReadReplyChunk()
	assertEqual(t, *reply, "\r\n")
	reply = c.ReadReplyChunk()
	assertEqual(t, reply, (*string)(nil))
}
