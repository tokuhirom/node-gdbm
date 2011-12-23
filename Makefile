all: gdbm.node

gdbm.node: gdbm.cc
	node-waf

test: gdbm.node
	node test.js

.PHONY: test all

