"use strict";

var gdbm = require('../build/Release/gdbm'),
    fs = require('fs'),
    meta = JSON.parse(fs.readFileSync('package.json', 'utf-8')),
    undefined;

exports.version = function (t) {
    t.equals(gdbm.version, meta.version);
    t.done();
};
