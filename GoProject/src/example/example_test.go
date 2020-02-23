package example

import "testing"

func TestAdd(t *testing.T) {
	r := add(2, 4)
	if r != 6 {
		t.Fatalf("add(2, 4) error ")
	}
	t.Logf("test add succ")
}
