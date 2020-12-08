const { deepStrictEqual } = require('assert');
const { getLinesFromFile } = require('../util/util');

const OUR_COLOR = 'shiny gold';

/**
 * interpret a rule line into a useful typedef
 */
function transformLineIntoRule(line) {
    const containerColor = line.substring(0, line.indexOf('bag')).trim();
    const contained = line
        .substring(line.indexOf('contain') + 8)
        .split(",")
        .map(s => s.trim())
        .filter(s => s !== "no other bags.")
        // contained bag text is like "1 muted yellow bag." or "2 white bags"
        .map(containedBagText => {
            const result = /^(\d+)\s([\w\s]+)\sbags?.?$/.exec(containedBagText);
            return createContainedRule(result[2], parseInt(result[1]));
        });
    return createRule(containerColor, contained);
}


/**
 * Recursively find all colors that contain the specified color
 */
function getAllColorsContainingColor(rules, color) {
    const containingColors = new Set();
    for(const rule of rules) {
        if (rule.contained.find(contained => contained.color === color)) {
            containingColors.add(rule.containerColor);
            for(const color of getAllColorsContainingColor(rules, rule.containerColor)) {
                containingColors.add(color);
            }
        }
    }
    return Array.from(containingColors);
}

/**
 * Recursively find the number of bags contained by a bag of a particular color
 */
function getNumberOfBagsContainedByColor(rules, color) {
    const rule = rules.find(rule => rule.containerColor === color);
    return rule.contained.reduce(
        (bagCount, containedRule) => bagCount + containedRule.number + containedRule.number * getNumberOfBagsContainedByColor(rules, containedRule.color),
        0
    );
}

function main() {
    const rules = getLinesFromFile('7.txt')
        .map(transformLineIntoRule);
    const colors = getAllColorsContainingColor(rules, OUR_COLOR).length;
    console.log(`Part 1: ${colors} bags contain the color "${OUR_COLOR}"`)
    const childBags = getNumberOfBagsContainedByColor(rules, OUR_COLOR);
    console.log(`Part 2: Our ${OUR_COLOR} bag holds ${childBags} bags`);
}

/**
 * consistent typedef we will use to describe rules
 */
function createRule(containerColor, contained) {
    return {
        containerColor,
        contained,
    };
}

/**
 * Consistent typedef that describes one color contained by another color
 */
function createContainedRule(color, number) {
    return {
        color,
        number,
    };
}

function test() {
    deepStrictEqual(
        transformLineIntoRule('light red bags contain 1 bright white bag, 2 muted yellow bags.'),
        { containerColor: 'light red', contained: [{ color: 'bright white', number: 1 }, { color: 'muted yellow', number: 2 }]},
        'transformLineIntoRule failed'
    );
    const exampleRules = getLinesFromFile('example.txt')
        .map(transformLineIntoRule);
    deepStrictEqual(getAllColorsContainingColor(exampleRules, OUR_COLOR).sort(), [
        'bright white', 'dark orange', 'light red', 'muted yellow',
    ]);
    deepStrictEqual(getNumberOfBagsContainedByColor(exampleRules, OUR_COLOR), 32);    
}

test();
main();
