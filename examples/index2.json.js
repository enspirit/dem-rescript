var fs = require('fs');
var home = require('os').homedir();
var dir = home + "/dem-bs/examples/";
var contents = fs.readFileSync(dir + "index.json", 'utf8');
var parsed = JSON.parse(contents);
JSON.stringify(parsed);
