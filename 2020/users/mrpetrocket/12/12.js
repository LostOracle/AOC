const { deepStrictEqual } = require('assert');
const { getLinesFromFile } = require('../util/util');
const { addVector, createVector, rotateAroundOrigin } = require('./vector');

function createInstruction(command, value) {
    return {
        command,
        value,
    };
}

function createDefaultShip() {
    return {
        direction: 'E',
        position: createVector(0, 0),
    }
}

// execute instruction on the ship
// return new ship representing results of instruction
function executeInstructionPart1(instruction, ship) {
    const { command, value } = instruction;
    switch(command) {
        case 'N':
            return moveShip(ship, createVector(0, -value));
        case 'S':
            return moveShip(ship, createVector(0, value));
        case 'E':
            return moveShip(ship, createVector(value, 0));
        case 'W':
            return moveShip(ship, createVector(-value, 0));
        case 'F':
            return executeInstructionPart1(
                createInstruction(ship.direction, value),
                ship,
            );
        case 'L':
            return rotateShip(ship, -value);
        case 'R':
            return rotateShip(ship, value);
    }
}

function getInstructionFromLine(line) {
    const command = line.substring(0, 1);
    const value = parseInt(line.substring(1));
    return createInstruction(command, value);
}

function getInstructionsFromLines(lines) {
    return lines.map(getInstructionFromLine);
}

function getManhattanDistance(vector) {
    return Math.abs(vector.x) + Math.abs(vector.y);
}

// move ship on vector. vector = [x, y] where:
// +x = east, -x = west
// +y = south, -y = north
// vector can only be in one direction right now
function moveShip(ship, vector) {
    return {
        ...ship,
        position: addVector(ship.position, vector),
    };
}

function part1(filename) {
    const instructions = getInstructionsFromLines(getLinesFromFile(filename));
    let ship = createDefaultShip();
    for(instruction of instructions) {
        ship = executeInstructionPart1(instruction, ship);
    }
    return getManhattanDistance(ship.position);
}

function part2(filename) {
    // waypoint is always relative to the ship
    let waypoint = createVector(10, -1);
    // ship doesn't have a concept of direction anymore so it's just a position
    let ship = createVector(0, 0);
    const instructions = getInstructionsFromLines(getLinesFromFile(filename));
    for(instruction of instructions) {
        const { command, value } = instruction;
        switch(command) {
            case 'N':
                waypoint = addVector(waypoint, createVector(0, -value));
                break;
            case 'S':
                waypoint = addVector(waypoint, createVector(0, value));
                break;
            case 'E':
                waypoint = addVector(waypoint, createVector(value, 0));
                break;
            case 'W':
                waypoint = addVector(waypoint, createVector(-value, 0));
                break;
            case 'L':
                waypoint = rotateAroundOrigin(waypoint, -value);
                break;
            case 'R':
                waypoint = rotateAroundOrigin(waypoint, value);
                break;
            case 'F':
                ship = addVector(ship, createVector(waypoint.x * value, waypoint.y * value));
                break;
        }
    }
    return getManhattanDistance(ship);
}

// rotate ship
// ship only rotates by increments of 90 degrees so we can cheat
// there is no cow level
// return updated ship
function rotateShip(ship, degrees) {
    const clockwiseDirections = ['N', 'E', 'S', 'W'];
    if (degrees < 0) degrees = 360 - Math.abs(degrees);
    const increments = degrees / 90;
    const currentDirectionIndex = clockwiseDirections.indexOf(ship.direction);
    return {
        ...ship,
        direction: clockwiseDirections[(currentDirectionIndex + increments) % 4],
    };
}

function test() {
    deepStrictEqual(getInstructionFromLine('F25'), {command:'F', value:25});
    deepStrictEqual(moveShip(createDefaultShip(), createVector(25, 0)), {direction:'E',position:{x:25,y:0}});
    deepStrictEqual(executeInstructionPart1({command: 'F', value: 25}, createDefaultShip()), {direction:'E',position:{x: 25, y: 0}});
    deepStrictEqual(rotateShip(createDefaultShip(), -270), {direction:'S',position:{x:0,y:0}});
    deepStrictEqual(part1('example.txt'), 25);
    deepStrictEqual(part1('12.txt'), 1565);
    deepStrictEqual(part2('example.txt'), 286);
}

function main() {
    const part1Distance = part1('12.txt');
    console.log(`Part 1: Manhattan distance is ${part1Distance}`);
    const part2Distance = part2('12.txt');
    console.log(`Part 2: Manhattan distance is ${part2Distance}`);
}

test();
main();
