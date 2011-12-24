all: gdbm.node

gdbm.node: gdbm.cc
	node-waf

test: gdbm.node
	nodeunit test.js

deps:
	npm install nodeunit

.PHONY: test all

