all: gdbm.node

build/config.log:
	node-waf configure

version.h: package.json
	node author/version.js

gdbm.node: gdbm.cc build/config.log version.h package.json
	node-waf

test: gdbm.node
	nodeunit test/

deps:
	npm install

clean:
	rm -rf build/ hoge.db .lock-wscript version.h gdbm.node

push: test clean
	npm publish

.PHONY: test all clean push deps

