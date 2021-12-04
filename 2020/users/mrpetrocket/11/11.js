const { deepStrictEqual } = require('assert');
const { getLinesFromFile } = require('../util/util');

const EMPTY = 'L';
const OCCUPIED = '#';
const FLOOR = '.';

// seating grid is a 2d array that looks like the puzzle input
function getSeatingGridFromFile(filename) {
    const lines = getLinesFromFile(filename);
    return lines.map(line => line.split(""));
}

function countOccupiedSeats(seatingGrid) {
    let occupied = 0;
    for(let i=0; i<seatingGrid.length; i++) {
        for(let j=0; j<seatingGrid[0].length; j++) {
            if (seatingGrid[i][j] === OCCUPIED) occupied++;
        }
    }
    return occupied;
}

// sees if a seat is occupied. if coordinates don't exist, the seat is not occupied
function seatIsOccupied(seatingGrid, rowIndex, columnIndex) {
    if (rowIndex < 0 || rowIndex >= seatingGrid.length) return false;
    if (columnIndex < 0 || columnIndex >= seatingGrid[0].length) return false;
    if (seatingGrid[rowIndex][columnIndex] !== OCCUPIED) return false;
    return true;
}

function countOccupiedAdjacentSeats(seatingGrid, rowIndex, columnIndex) {
    let adjacentSeatCoordinates = [
        [rowIndex - 1, columnIndex - 1],
        [rowIndex - 1, columnIndex],
        [rowIndex - 1, columnIndex + 1],
        [rowIndex, columnIndex - 1],
        [rowIndex, columnIndex + 1],
        [rowIndex + 1, columnIndex - 1],
        [rowIndex + 1, columnIndex],
        [rowIndex + 1, columnIndex + 1],
    ];
    return adjacentSeatCoordinates.reduce((total, coordinate) => {
        if (seatIsOccupied(seatingGrid, coordinate[0], coordinate[1])) return total + 1;
        return total;
    }, 0);
}

// find occupied seats on all eight "sight vectors" from the location
function countOccupiedSeatsInSight(seatingGrid, rowIndex, columnIndex) {
    const sightVectors = [
        [-1, -1],
        [-1, 0],
        [-1, 1],
        [0, -1],
        [0, 1],
        [1, -1],
        [1, 0],
        [1, 1],
    ];
    return sightVectors.reduce((total, vector) => {
        let sightRow = rowIndex, sightColumn = columnIndex;
        while(true) {
            sightRow += vector[0];
            sightColumn += vector[1];
            if (sightRow < 0 || sightRow >= seatingGrid.length) break;
            if (sightColumn < 0 || sightColumn >= seatingGrid[0].length) break;
            if (seatingGrid[sightRow][sightColumn] === EMPTY) return total;
            if (seatIsOccupied(seatingGrid, sightRow, sightColumn)) return total + 1;
        }
        return total;
    }, 0);
}

// simulate one round of seating activity
// return the updated seating grid
function simulateActivity(
    seatingGrid,
    adjacentSeatCounterFn,
    numberOfNearbySeatsToDisturbOccupant
) {
    let numberOfChanges = 0;
    const newSeatingGrid = seatingGrid.map((row, rowIndex) => {
        return row.map((value, columnIndex) => {
            const occupiedAdjacentSeats = adjacentSeatCounterFn(seatingGrid, rowIndex, columnIndex);

            // "If a seat is empty (L) and there are no occupied seats adjacent to it, the seat becomes occupied."
            if (value === EMPTY && occupiedAdjacentSeats === 0) {
                numberOfChanges++;
                return OCCUPIED;
            }

            // "If a seat is occupied (#) and four or more seats adjacent to it are also occupied, the seat becomes empty."
            if (value === OCCUPIED && occupiedAdjacentSeats >= numberOfNearbySeatsToDisturbOccupant) {
                numberOfChanges++;
                return EMPTY;
            }

            // no change to this position
            return value;
        });
    });
    return createSimulateActivityResults(numberOfChanges, newSeatingGrid);
}

function simulateActivityPart1(seatingGrid) {
    return simulateActivity(
        seatingGrid,
        countOccupiedAdjacentSeats,
        4
    );
}

function simulateActivityPart2(seatingGrid) {
    return simulateActivity(
        seatingGrid,
        countOccupiedSeatsInSight,
        5
    );
}

// simulate as many rounds of seating activity as it takes
// for the number of occupied seats to stop changing
// return the simulateActivity results for the last round
function simulateActivityUntilPassengersSettleDown(
    seatingGrid,
    simulateActivityFn,
) {
    while(true) {
        const results = simulateActivityFn(seatingGrid);
        if (results.numberOfChanges === 0) return results;
        seatingGrid = results.seatingGrid;
    }
}

function createSimulateActivityResults(numberOfChanges, seatingGrid) {
    return {
        numberOfChanges,
        seatingGrid,
    };
}

function test() {
    // test part 1
    const startingSeatingGrid = getSeatingGridFromFile('example.txt');
    const part1Results = simulateActivityUntilPassengersSettleDown(
        startingSeatingGrid,
        simulateActivityPart1,
    );
    deepStrictEqual(countOccupiedSeats(part1Results.seatingGrid), 37);

    // test the "count occupied seats in sight" fn
    deepStrictEqual(countOccupiedSeatsInSight(getSeatingGridFromFile('vectors1.txt'), 4, 3), 8);
    deepStrictEqual(countOccupiedSeatsInSight(getSeatingGridFromFile('vectors2.txt'), 1, 2), 0);
    deepStrictEqual(countOccupiedSeatsInSight(getSeatingGridFromFile('vectors3.txt'), 3, 3), 0);

    // test part 2
    const part2Results = simulateActivityUntilPassengersSettleDown(
        startingSeatingGrid,
        simulateActivityPart2,
    );
    deepStrictEqual(countOccupiedSeats(part2Results.seatingGrid), 26);
}

function main() {
    const startingSeatingGrid = getSeatingGridFromFile('11.txt');

    // part 1
    const part1Results = simulateActivityUntilPassengersSettleDown(
        startingSeatingGrid,
        simulateActivityPart1,
    );
    const occupiedSeats = countOccupiedSeats(part1Results.seatingGrid);
    console.log(`Part 1: ${occupiedSeats} seats occupied`);

    // part 2
    const part2Results = simulateActivityUntilPassengersSettleDown(
        startingSeatingGrid,
        simulateActivityPart2,
    );
    const occupiedSeatsPart2 = countOccupiedSeats(part2Results.seatingGrid);
    console.log(`Part 2: ${occupiedSeatsPart2} seats occupied`);
}

test();
main();
