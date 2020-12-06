const assert = require('assert');
const fs = require('fs');

/**
 * helper that combines getLinesFromFile and getLineGroups
 */
function getGroupsFromFile(filename) {
    return getLineGroups(getLinesFromFile(filename));
}

/**
 * split lines into groups
 * groups are divided by blank lines
 */
function getLineGroups(lines) {
    const groups = [[]];
    for(line of lines) {
        if (line == "") {
            groups.push([]);
        } else {
            groups[groups.length - 1].push(line);
        }
    }
    return groups;
}

/**
 * readLines but moved here
 */
function getLinesFromFile(filename) {
    return fs.readFileSync(filename, { encoding: 'UTF-8' })
        .split('\r\n');
}

assert.deepStrictEqual(getLineGroups(['a', 'b', '', 'c', '', 'd', 'e']), [['a', 'b'], ['c'], ['d', 'e']]);

module.exports = {
    getLinesFromFile,
    getLineGroups,
    getGroupsFromFile,
};
