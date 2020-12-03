const fs = require('fs');

module.exports = function readlines(filename) {
    return fs.readFileSync(filename, {encoding:'UTF-8'})
        .split('\r\n');
}
