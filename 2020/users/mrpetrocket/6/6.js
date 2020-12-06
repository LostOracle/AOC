const assert = require('assert');
const { getGroupsFromFile } = require('../util/util');

function part1(filename) {
    // combine lines into groups, then combine member answers into 
    // single array of question chars for each group
    const answersByGroupAndMember = getGroupsFromFile(filename);
    const answersByGroup = answersByGroupAndMember
        .map(group => group.join("").split(""));
    
    // remove duplicates for groups
    const uniqueAnswersByGroup = answersByGroup.map(removeDuplicates);
    
    // count unique answers for each group, sum across all groups
    return sumChildElements(uniqueAnswersByGroup);
}

function part2(filename) {
    // get answers as group[member line]
    const answersByGroupAndMember = getGroupsFromFile(filename);

    // get common answers among group members as group[answers array]
    const commonAnswersPerGroup = answersByGroupAndMember
        // turn each group member char string into an array
        .map(group => group.map(memberString => memberString.split("")))
        // find common elements among all group members
        .map(group => {
            return group.reduce((commonAnswers, memberAnswers) => {
                return commonAnswers.filter(commonAnswer => memberAnswers.includes(commonAnswer));
            }, group[0]);
        });
    
    // sum common answer counts from all groups
    return sumChildElements(commonAnswersPerGroup);

}

function main() {
    const part1Answer = part1('6.txt');
    console.log(`Part 1: ${part1Answer}`);
    const part2Answer = part2('6.txt');
    console.log(`Part 2: ${part2Answer}`);
}

function test() {
    assert.deepStrictEqual(removeDuplicates(['1', '2']), ['1', '2']);
    assert.deepStrictEqual(removeDuplicates(['1', '2', '1', '5', '3', '3', '1', '4', '5']), ['2', '3', '1', '4', '5'])
    assert.strictEqual(part1('example.txt'), 11);
    assert.strictEqual(part2('example.txt'), 6);
}

function removeDuplicates(arr) {
    return arr.filter((el, index) => arr.indexOf(el, index + 1) === -1);
}

/**
 * sum elements of a nested array
 */
function sumChildElements(arr) {
    return arr.reduce((sum, secondLevelArray) => sum + secondLevelArray.length, 0)
}

test();
main();
