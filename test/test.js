"use strict";

var gdbm = require('../index'),
    fs = require('fs'),
    undefined;

exports.setUp = function (callback) {
    fs.unlink('hoge.db', function (err) {
        // ignore error
        callback();
    });
};
exports.tearDown = function (callback) {
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
    t.ok(ret);
    if (!ret) {
        console.log(db.strerror());
        console.log('errno: ' + db.errno());
    }
    db.sync();
    t.ok(db.fdesc());
    t.ok((''+db.fdesc()).match(/^[0-9]+$/));
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

exports.exists_delete = function (t) {
    var db = new gdbm.GDBM();

    var ret = db.open('hoge.db', 0, gdbm.GDBM_WRCREAT);
    t.ok(ret);
    t.ok(!db.exists('dan'));
    db.store('dan', 'kogai');
    t.ok(db.exists('dan'));
    t.ok(db.delete('dan'));
    t.ok(!db.delete('dan'));
    t.ok(!db.exists('dan'));
    db.reorganize();
    db.close();

    t.done();
};

exports.iterator = function (t) {
    var db = new gdbm.GDBM();

    var ret = db.open('hoge.db', 0, gdbm.GDBM_WRCREAT);
    t.ok(ret);

    var key = db.firstkey();
    var ret = [];
    while (key) {
        ret.push(key);
        key = db.nextkey(key);
    }
    t.equal(ret.sort().join(','), '');

    db.close();

    t.done();
};

exports.iterator2 = function (t) {
    var db = new gdbm.GDBM();

    var ret = db.open('hoge.db', 0, gdbm.GDBM_WRCREAT);
    t.ok(ret);
    db.store('a', '1');
    db.store('b', '2');
    db.store('c', '3');

    var key = db.firstkey();
    var ret = [];
    while (key) {
        ret.push(key);
        key = db.nextkey(key);
    }
    t.equal(ret.sort().join(','), 'a,b,c');

    db.close();

    t.done();
};

exports.i18n = function (t) {
    var db = new gdbm.GDBM();

    var ret = db.open('hoge.db', 0, gdbm.GDBM_WRCREAT);
    t.ok(!db.exists('いやんばかん'));
    db.store('いやんばかん', 'そこはだめよ');
    t.equal(db.fetch('いやんばかん'), 'そこはだめよ');
    t.ok(db.exists('いやんばかん'));
    db.delete('いやんばかん');
    t.ok(!db.exists('いやんばかん'));

    {
        db.store('にくまん', 'おいしい');
        db.store('あんまん', 'あまい');

        var key = db.firstkey();
        var ret = [];
        while (key) {
            ret.push(key);
            key = db.nextkey(key);
        }
        t.equal(ret.sort().join(','), 'あんまん,にくまん');
    }

    db.close();

    t.done();
};

exports.errHandling = function (t) {
    var db = new gdbm.GDBM();

    // no exception on closing
    db.close();

    t.throws(function () { db.sync(); });
    t.throws(function () { db.store('hoge', 'fuga'); });
    t.throws(function () { db.fetch('hoge'); });
    t.throws(function () { db.exists('hoge'); });
    t.throws(function () { db.reorganize(); });
    t.throws(function () { db.fdesc(); });
    t.throws(function () { db.firstkey(); });
    t.throws(function () { db.nextkey('hoge'); });

    t.done();
};

exports.argsErrorHandling = function (t) {
    var db = new gdbm.GDBM();

    var ret = db.open('hoge.db', 0, gdbm.GDBM_WRCREAT);
    t.throws(function () { db.store(); });
    t.throws(function () { db.fetch(); });
    t.throws(function () { db.exists(); });
    t.throws(function () { db.nextkey(); });

    t.done();
};

