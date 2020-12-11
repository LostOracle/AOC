const { deepStrictEqual } = require('assert');
const { getIntsFromFile } = require('../util/util');

// get all adapters, wall outlet and device joltages as a sorted array
function getChargingStepsFromFile(filename) {
    const adapters = getIntsFromFile(filename);
    adapters.push(0);  // wall outlet is 0
    numericSort(adapters);
    adapters.push(adapters[adapters.length-1] + 3); // your device is your highest adapter + 3
    return adapters;
}

function part1(filename) {
    const adapters = getChargingStepsFromFile(filename);

    let differencesOf1 = 0, differencesOf3 = 0;
    for(let i=1; i<adapters.length; i++) {
        const difference = adapters[i] - adapters[i-1];
        if (difference === 1) differencesOf1++;
        if (difference === 3) differencesOf3++;
    }

    return differencesOf1 * differencesOf3;
}

function numericSort(ar) {
    ar.sort((a,b) => {
        if (a<b) return -1;
        if (a>b) return 1;
        return 0;
    });
}

deepStrictEqual(part1('example1.txt'), 35);
deepStrictEqual(part1('example2.txt'), 220);

console.log(`Part 1: ${part1('10.txt')}`);

deepStrictEqual(part2('example1.txt'), 8);
deepStrictEqual(part2('example2.txt'), 19208);

// taken 1hr so far, no exit
// console.log(`Part 2: ${part2('10.txt')}`);

deepStrictEqual(splitIntoGroupsSeparatedBy3Gaps([
    0,  1,  4,  5,  6,
    7, 10, 11, 12, 15,
    16, 19
]), [[0, 1], [4, 5, 6, 7], [10, 11, 12], [15, 16], [19]]);
deepStrictEqual(part2_2('example1.txt'), 8);
deepStrictEqual(part2_2('example2.txt'), 19208);

console.log(`Part 2: ${part2_2('10.txt')}`);

// straight up brute force the problem.
function part2(filename) {
    const chargingSteps = getChargingStepsFromFile(filename);
    let permutations = getPermutations(chargingSteps, 0);
    return permutations;
}

function getPermutations(chargingSteps, currentIndex) {
    // at this level, find what can connect to us
    // if the top level element (the device) can connect to us, that's a permutation
    let permutations = 0;
    const currentChargingStep = chargingSteps[currentIndex];
    for(let i = currentIndex + 1; i < chargingSteps.length; i++) {
        if (chargingSteps[i] <= currentChargingStep + 3) {
            if (i === chargingSteps.length - 1) permutations++;
            else permutations += getPermutations(chargingSteps, i);
        } else break;
    }
    return permutations;
}

// when there are "3 gaps" between charging steps, there can be only one permutation
// divide the problem into multiple groups of charging steps existing between 3 gaps
// then solve permutations for each group of charging steps
function part2_2(filename) {
    const chargingSteps = getChargingStepsFromFile(filename);
    chargingSteps.pop();    // remove the device charging step from the end since it's always a 3 gap
    const groupsSeparatedBy3Gaps = splitIntoGroupsSeparatedBy3Gaps(chargingSteps);
    return groupsSeparatedBy3Gaps.reduce((total, group) => {
        // if there are 2 or fewer numbers in the group it's still just one permutation
        if (group.length <= 2) return total;
        // now here's a real group we have to solve for. brute force find all permutations for the group.
        return total * getPermutations(group, 0);
    }, 1);
}

// split into groups separated by numbers that differ by 3
function splitIntoGroupsSeparatedBy3Gaps(numbers) {
    const trees = [[numbers[0]]];
    for(let i=1; i<numbers.length; i++) {
        if (numbers[i] - numbers[i-1] === 3) {
            trees.push([]);
        }
        trees[trees.length - 1].push(numbers[i]);
    }
    return trees;
}
