const assert = require('assert');

/**
 * iterate over chars, halving range for each char
 * for each char, if it's 'rightChar' we take the upper half of the range, otherwise the lower
 */
function binaryPartition(chars, range, rightChar) {
    let base = 0;
    for(char of chars) {
        if (char === rightChar) base += range / 2;
        range = Math.floor(range / 2);
    }
    return base;
}

function getSeatRow(s) {
    const chars = s.substring(0, 8)
    return binaryPartition(chars, 128, 'B');
}

function getSeatColumn(s) {
    const chars = s.substring(7);
    return binaryPartition(chars, 8, 'R');
}

function getSeatId(s) {
    const row = getSeatRow(s), column = getSeatColumn(s);
    return row * 8 + column;
}

function max(numbers) {
    return Math.max.apply(null, numbers);
}

function test() {
    assert.strictEqual(getSeatRow('FFFFFFFRRR'), 0);
    assert.strictEqual(getSeatRow('FFFBBBFRRR'), 14);
    assert.strictEqual(getSeatRow('FBFBBFBRLR'), 45);
    assert.strictEqual(getSeatRow('BFFFBBFRRR'), 70);
    assert.strictEqual(getSeatRow('BBFFBBFRLL'), 102);
    assert.strictEqual(getSeatRow('BBBBBBBRRR'), 127);

    assert.strictEqual(getSeatColumn('BBFFBBFRLL'), 4);
    assert.strictEqual(getSeatColumn('BFFFBBFRRR'), 7);
    assert.strictEqual(getSeatColumn('FFFBBBFRRR'), 7);

    assert.strictEqual(getSeatId('BFFFBBFRRR'), 567);
    assert.strictEqual(getSeatId('FFFBBBFRRR'), 119);
    assert.strictEqual(getSeatId('BBFFBBFRLL'), 820);

    assert.strictEqual(max([5, 40000, 26]), 40000);
}

function main() {
    const seatIds = require('../util/readlines')('5.txt').map(getSeatId);
    const maxSeatId = max(seatIds);
    console.log(`Highest Seat ID: ${maxSeatId}`);

    // sort seats by seat id, then find the seat id with the next seat id being 2 away.
    // ours fits in between.
    const sorted = seatIds.sort();
    for(index = 0; index < sorted.length - 1; index++) {
        if (sorted[index] === sorted[index + 1] - 2) {
            console.log(`My Seat ID: ${sorted[index] + 1}`);
            break;
        }
    }
}

test();
main();
