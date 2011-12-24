"use strict";

var gdbm = require('./build/Release/gdbm');

console.log(gdbm.gdbm()); // 'world'

exports.gdbm_version = function (t) {
    console.log(gdbm.gdbm_version);
    t.ok(gdbm.gdbm_version);
    t.done();
};

