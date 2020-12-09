const { deepStrictEqual } = require('assert');
const { getLinesFromFile } = require('../util/util');

// numberSlice size is the preamble length + 1; the last number is the number in question
function numberFitsEncoding(numberSlice) {
    const preambleLength = numberSlice.length - 1;
    const number = numberSlice[preambleLength];
    for(let i=0; i<preambleLength; i++) {
        for(let j=0; j<preambleLength; j++) {
            if (numberSlice[i] + numberSlice[j] === number) return true;
        }
    }
    return false;
}

// find number in file that doesn't match XMAS rules
function findInvalidXmasNumber(numbers, preambleLength) {
    for(let i=preambleLength; i<numbers.length; i++) {
        if (!numberFitsEncoding(numbers.slice(i - preambleLength, i+1))) {
            return numbers[i];
        }
    }
    throw new Error('did not find invalid XMAS number');
}

// find contiguous range of numbers that add up to number
function findRangeAddingUpToNumber(numbers, number) {
    for(let i=0; i<numbers.length; i++) {
        let rangeSum = 0;
        for(let j=i; j<numbers.length; j++) {
            rangeSum += numbers[j];
            if (rangeSum === number) {
                // we got it - return the lucky numbers
                return numbers.slice(i, j + 1);
            } else if (rangeSum > number) {
                // overshot - no continuous range starting at i
                break;
            }
        }
    }
    throw new Error('did not find useful range');
}

function getEncryptionWeaknessForRange(range) {
    range.sort();
    return range[0] + range[range.length - 1];
}

function getNumbersFromFile(filename) {
    return getLinesFromFile(filename).map(line => parseInt(line));
}

function test() {
    const numbers = getNumbersFromFile('example.txt');
    deepStrictEqual(findInvalidXmasNumber(numbers, 5), 127);
    deepStrictEqual(findRangeAddingUpToNumber(numbers, 127), [15, 25, 47, 40]);
    deepStrictEqual(getEncryptionWeaknessForRange([15, 25, 47, 40]), 62);
}

function main() {
    const numbers = getNumbersFromFile('9.txt');
    const part1 = findInvalidXmasNumber(numbers, 25);
    console.log(`Part 1: First invalid number is ${part1}`);
    const part2Numbers = findRangeAddingUpToNumber(numbers, part1);
    const encryptionWeakness = getEncryptionWeaknessForRange(part2Numbers);
    console.log(`Part 2: Encryption weakness is ${encryptionWeakness}`);
}

test();
main();
