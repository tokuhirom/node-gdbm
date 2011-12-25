all: gdbm.node

build/config.log:
	node-waf configure

gdbm.node: gdbm.cc build/config.log
	node-waf

test: gdbm.node
	nodeunit test/

deps:
	npm install nodeunit

clean:
	rm -rf build/ hoge.db .lock-wscript

.PHONY: test all

