"use strict";

var gdbm = require('./build/Release/gdbm'),
    fs = require('fs'),
    undefined;

console.log(gdbm.gdbm()); // 'world'


exports.setUp = function (callback) {
    fs.unlink('hoge.db', function (err) {
        // ignore error
        callback();
    });
};

exports.gdbm_version = function (t) {
    console.log(gdbm.gdbm_version);
    t.ok(gdbm.gdbm_version);
    t.done();
};

exports.files = function (t) {
    var db = new gdbm.GDBM();
    t.equal(db.errno(), 0);
    t.ok(gdbm.GDBM_WRITER);
    var ret = db.open('hoge.db', 0, gdbm.GDBM_WRCREAT);
    console.log(db.strerror());
    console.log('errno: ' + db.errno());
    db.sync();
    t.ok(ret);
    db.close();

    t.done();
};

exports.store_fetch = function (t) {
    var db = new gdbm.GDBM();

    var ret = db.open('hoge.db', 0, gdbm.GDBM_WRCREAT);
    t.ok(ret);
    if (!ret) {
        console.log(db.strerror());
        console.log('errno: ' + db.errno());
    }
    db.store('dan', 'kogai');
    db.close();

    var db2 = new gdbm.GDBM();
    var ret = db2.open('hoge.db', 0, gdbm.GDBM_READER);
    t.ok(ret);
    t.equal(db2.fetch('dan'), 'kogai');
    db2.close();

    t.done();
};

exports.exists = function (t) {
    var db = new gdbm.GDBM();

    var ret = db.open('hoge.db', 0, gdbm.GDBM_WRCREAT);
    t.ok(ret);
    t.ok(!db.exists('dan'));
    db.store('dan', 'kogai');
    t.ok(db.exists('dan'));
    db.close();

    t.done();
};

