var fs = require('fs'),
    undefined;

var src = fs.readFileSync('../package.json', 'utf-8');
var dat = JSON.parse(src);
fs.writeFileSync('version.h', '#define NODE_GDBM_VERSION "' + dat.version + "\"\n");

