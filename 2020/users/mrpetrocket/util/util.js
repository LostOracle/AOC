const assert = require('assert');
const fs = require('fs');

function getIntsFromFile(filename) {
    return getLinesFromFile(filename).map(line => parseInt(line));
}

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

function getLinesFromFile(filename) {
    return fs.readFileSync(filename, { encoding: 'UTF-8' })
        .split('\n')
        .map(s => s.trim());
}

assert.deepStrictEqual(getLineGroups(['a', 'b', '', 'c', '', 'd', 'e']), [['a', 'b'], ['c'], ['d', 'e']]);

module.exports = {
    getIntsFromFile,
    getLinesFromFile,
    getLineGroups,
    getGroupsFromFile,
};
