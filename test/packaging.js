"use strict";

var gdbm = require('../index.js'),
    fs = require('fs'),
    meta = JSON.parse(fs.readFileSync('package.json', 'utf-8')),
    undefined;

exports.version = function (t) {
    t.equals(gdbm.version, meta.version);
    t.done();
};
