package main

import (
	"fmt"
)

const (
	_        = iota
	kb int64 = 1 << (10 * iota)
	mb
	gb
	tb
)

func main() {
	fmt.Println(kb, mb, gb, tb)
}
