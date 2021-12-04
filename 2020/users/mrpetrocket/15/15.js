const { deepStrictEqual } = require('assert');

/**
 * play the memory game to round gameLength (including startingNumbers.length)
 * returns an array of all numbers spoken
 */
function memoryGame(startingNumbers, gameLength) {
    const game = Array.from(startingNumbers);
    const spokenNumbers = new Map();
    for(let i=0; i<startingNumbers.length - 1; i++) spokenNumbers.set(startingNumbers[i], i);
    while(game.length < gameLength) {
        const lastSpokenNumber = game[game.length - 1];
        const lastOccurrenceIndex = spokenNumbers.get(lastSpokenNumber);
        if (lastOccurrenceIndex === undefined) {
            // new number
            game.push(0);
        } else {
            // number has been spoken before
            game.push(game.length - lastOccurrenceIndex - 1);
        }
        spokenNumbers.set(lastSpokenNumber, game.length - 2);
        // the 30mil run is taking a while so here's a progress meter
        if (game.length % 100000 === 0) console.log('game.length = ' + game.length);
    }
    return game;
}

function lastSpokenNumberInGame(game) {
    return game[game.length - 1];
}

function test() {
    memoryGame([0,3,6], 10);
    deepStrictEqual(lastSpokenNumberInGame(memoryGame([0,3,6], 10)), 0);
    const testArray = [
        {ar: [1, 3, 2], last: 1},
        {ar: [2, 1, 3], last: 10},
        {ar: [1, 2, 3], last: 27},
        {ar: [2, 3, 1], last: 78},
        {ar: [3, 2, 1], last: 438},
        {ar: [3, 1, 2], last: 1836},
    ];
    testArray.forEach(test => deepStrictEqual(lastSpokenNumberInGame(memoryGame(test.ar, 2020)), test.last));
}

function main() {
    const myInput = [11,0,1,10,5,19];
    const part1 = lastSpokenNumberInGame(memoryGame(myInput, 2020));
    console.log(`Part 1: ${part1}`);
    const part2 = lastSpokenNumberInGame(memoryGame(myInput, 30000000));
    console.log(`Part 2: ${part2}`);
}

test();
main();
