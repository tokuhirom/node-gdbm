all: gdbm.node

build/config.log:
	node-waf configure

version.h:
	node author/version.js

gdbm.node: gdbm.cc build/config.log version.h
	node-waf

test: gdbm.node
	nodeunit test/

deps:
	npm install

clean:
	rm -rf build/ hoge.db .lock-wscript version.h

push: test clean
	npm publish

.PHONY: test all clean push deps

