const { deepStrictEqual } = require('assert');
const { getLinesFromFile } = require('../util/util');

const ACTIVE = '#', INACTIVE = '.';
// hardcoded size of each dimension of space
// I found this value by trial and error
const SPACE_SIZE = 20;

function countActiveCubesInSpace(space) {
    let activeCubes = 0;
    forEachElementInNestedArray(space.array, (cube) => {
        if (cube === ACTIVE) activeCubes++;
    });
    return activeCubes;
}

// createSpace but 4d
// https://youtu.be/j0GZ3qSV9s0?t=70
function createHyperspace() {
    return new NDimensionalSpace(4);
}

// "n" dimensional array with all dimensions of size "size"
function createNDimensionArray(n, size, defaultValue) {
    const dimension = Array(size).fill(null);
    if (n === 1)
        return dimension.fill(defaultValue);
    else
        return dimension.map(() => createNDimensionArray(n - 1, size, defaultValue));
}

// create a 3d space of hardcoded size
// by default each piece of space is INACTIVE
function createSpace() {
    return new NDimensionalSpace(3);
}

// execute a function for each element in a nested array
// fn should have this signature: fn(value, [coordinates])
function forEachElementInNestedArray(array, fn, dimensionIndices) {
    array.forEach((element, index) => {
        const updatedDimensionIndices = !!dimensionIndices
            ? dimensionIndices.concat([index])
            : [index];
        if (Array.isArray(element)) {
            // need to go deeper
            forEachElementInNestedArray.call(null, element, fn, updatedDimensionIndices);
        } else {
            // we have a real, non-array element
            fn.call(null, element, updatedDimensionIndices);
        }
    });
}

// get 2d char array of the starting slice of space from a file
function getStartingSliceFromFile(filename) {
    return getLinesFromFile(filename)
        .map(line => line.split(""));
}

// return the number of active neighbors for a given cube
// neighbor = any cube whose coordinates differ from this cube by max 1
// space is a 3d array where each element represents the state of a cube
function getNumberOfActiveNeighbors3d([x, y, z], space) {
    let activeNeighbors = 0;
    for(let xScan = Math.max(0, x - 1); xScan <= Math.min(space.length - 1, x + 1); xScan++) {
        for(let yScan = Math.max(0, y - 1); yScan <= Math.min(space[xScan].length - 1, y + 1); yScan++) {
            for(let zScan = Math.max(0, z - 1); zScan <= Math.min(space[yScan].length - 1, z + 1); zScan++) {
                if (xScan === x && yScan === y && zScan === z) continue;
                if (space[xScan][yScan][zScan] === ACTIVE) activeNeighbors++;
            }
        }
    }
    return activeNeighbors;
}

// same idea as its 3d counterpart
function getNumberOfActiveNeighbors4d([w, x, y, z], space) {
    let activeNeighbors = 0;
    for(let wScan = Math.max(0, w - 1); wScan <= Math.min(space.length - 1, w + 1); wScan++) {
        for(let xScan = Math.max(0, x - 1); xScan <= Math.min(space[wScan].length - 1, x + 1); xScan++) {
            for(let yScan = Math.max(0, y - 1); yScan <= Math.min(space[wScan][xScan].length - 1, y + 1); yScan++) {
                for(let zScan = Math.max(0, z - 1); zScan <= Math.min(space[wScan][xScan][yScan].length - 1, z + 1); zScan++) {
                    if (wScan === w && xScan === x && yScan === y && zScan === z) continue;
                    if (space[wScan][xScan][yScan][zScan] === ACTIVE) activeNeighbors++;
                }
            }
        }
    }
    return activeNeighbors;
}


// puts the starting slice at approximately the center of space
function insertStartingSliceIntoSpace(slice, space) {
    // the idea here is that the slice is a 2d chunk
    // so first find the bottom two dimensions of space,
    // then insert our slice at the center of those two dimensions
    let bottomTwoDimensions = space.array;
    for(let i=0; i<space.n - 2; i++) {
        bottomTwoDimensions = bottomTwoDimensions[Math.floor(bottomTwoDimensions.length / 2)];
    }
    y = Math.floor(bottomTwoDimensions.length / 2 - slice.length / 2),
    z = Math.floor(bottomTwoDimensions[0].length / 2 - slice[0].length / 2);
    slice.forEach((row, rowIndex) => {
        row.forEach((colValue, colIndex) => {
            bottomTwoDimensions[y + rowIndex][z + colIndex] = colValue;
        });
    });
}

class NDimensionalSpace {
    constructor(n) {
        this.array = createNDimensionArray(n, SPACE_SIZE, INACTIVE);
        this.n = n;
    }
}

function part1(filename) {
    return simulation(filename, createSpace());
}

function part2(filename) {
    return simulation(filename, createHyperspace());
}

function setElementInNestedArray(array, coordinates, value) {
    let position = array;
    for(let i=0; i<coordinates.length - 1; i++) {
        position = position[coordinates[i]];
    }
    position[coordinates[coordinates.length - 1]] = value;
}

function shouldCubeBeActive(isCurrentlyActive, numberOfActiveNeighbors) {
    if (isCurrentlyActive) {
        // stays active
        if ([2, 3].includes(numberOfActiveNeighbors)) return true;
    } else {
        // becomes active
        if (numberOfActiveNeighbors === 3) return true;
    }
    return false;
}

// calculate what those fancy cubes do in space for one round
// returns space as it stands after the round
function simulateRound(space) {
    const newSpace = new NDimensionalSpace(space.n);
    forEachElementInNestedArray(space.array, (cube, coordinates) => {
        const isActive = cube === ACTIVE;
        const numberOfActiveNeighbors = (space.n === 3)
            ? getNumberOfActiveNeighbors3d(coordinates, space.array)
            : getNumberOfActiveNeighbors4d(coordinates, space.array);
        if (shouldCubeBeActive(isActive, numberOfActiveNeighbors)) {
            setElementInNestedArray(newSpace.array, coordinates, ACTIVE);
        }
    });
    return newSpace;
}

function simulateRounds(space, numberOfRounds) {
    for(let round = 0; round < numberOfRounds; round++) {
        space = simulateRound(space);
    }
    return space;
}

// simulation control for parts 1 and 2
// run 6 rounds against provided starting N-dimensional space (3d or 4d)
// return number of active cubes at the end
function simulation(filename, space) {
    const numberOfRounds = 6;
    insertStartingSliceIntoSpace(getStartingSliceFromFile(filename), space);
    const endingSpace = simulateRounds(space, numberOfRounds);
    return countActiveCubesInSpace(endingSpace);
}

function test() {
    deepStrictEqual(part1('example.txt'), 112);
}

function main() {
    console.log(`Part 1: ${part1('17.txt')} active cubes after 6 rounds`);
    console.log('Part 2 will take a little while; don\'t worry');
    console.log(`Part 2: ${part2('17.txt')} active cubes after 6 rounds`);
}

test();
main();
