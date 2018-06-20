package eredis

import (
	"fmt"
	"testing"
)

func TestEmbeddedRedis(t *testing.T) {
	err := Init()
	if err != nil {
		fmt.Println("Failed to initialize!")
		return
	}

	fmt.Println("EmbeddedRedis initialized")

	c := CreateClient()
	defer c.Release()

	fmt.Println("Got an embedded client!")
	c.PrepareRequest([]string{"SET", "mykey", "myval"})
	reply := c.Execute()

	fmt.Printf("Reply: '%s'\n", reply)
}
