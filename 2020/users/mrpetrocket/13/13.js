const { getLinesFromFile } = require('../util/util');
const { deepStrictEqual } = require('assert');

function createBus(id, offset) {
    return {id, offset};
}

function createWaitingTimeResult(bus, waitingTime) {
    return { bus, waitingTime };
}

/**
 * get buses from second line of input
 */
function getBuses(line) {
    return line.split(",")
        .map((bus, index) => {
            if (bus !== 'x') {
                return createBus(parseInt(bus), index);
            }
            return 'x';
        })
        .filter(bus => bus !== 'x');
}

function getBusesFromFile(filename) {
    const lines = getLinesFromFile(filename);
    return getBuses(lines[1]);
}

/**
 * Match each bus against an aggregate interval of when the previous buses will depart on time.
 * Each time we match a new bus we add that bus to the interval so we're always comparing
 * only two buses at a time.
 */
function getTimeWhenAllBusesDepartOnSchedule(buses) {
    let interval = buses[0];
    for(let matchIndex = 1; matchIndex < buses.length; matchIndex++) {
        interval = matchBusToInterval(buses[matchIndex], interval.id, interval.offset)
    }
    return interval.offset;
}

/**
 * bus leaves at an interval of the value "bus"
 * readyToDepartTimestamp is when we are ready to catch the bus
 */
function getWaitingTimeForBus(bus, readyToDepartTimestamp) {
    return Math.ceil(readyToDepartTimestamp / bus) * bus - readyToDepartTimestamp;
}

/**
 * figure out when a bus departs "on schedule" relative to an interval
 * we have to match twice to get the period
 * return a bus but it's basically a period function describing the "base T" where bus and interval match
 */
function matchBusToInterval(bus, interval, startingTime) {
    let t = startingTime;
    let foundMatch = false;
    const timeInterval = interval;
    let matches = [];
    while(matches.length < 2) {
        foundMatch = (t + bus.offset) % bus.id === 0;
        if (foundMatch) matches.push(t);
        t += timeInterval;
    }
    return createBus(matches[1] - matches[0], matches[0]);
}

function part1(filename) {
    const lines = getLinesFromFile(filename);
    const readyToDepartTimestamp = parseInt(lines[0]);
    const buses = lines[1].split(",").filter(line => line !== 'x');
    
    const smallestWaitingTime = buses.reduce((smallestWaitingTime, bus) => {
        const waitingTimeForBus = getWaitingTimeForBus(bus, readyToDepartTimestamp);
        if (
            smallestWaitingTime === null
            || waitingTimeForBus < smallestWaitingTime.waitingTime 
        ) return createWaitingTimeResult(bus, waitingTimeForBus);
        return smallestWaitingTime;
    }, null);

    return smallestWaitingTime.bus * smallestWaitingTime.waitingTime;
}

function part2(filename) {
    return getTimeWhenAllBusesDepartOnSchedule(getBusesFromFile(filename));
}

function test() {
    deepStrictEqual(getWaitingTimeForBus(59, 939), 5);
    deepStrictEqual(part1('example.txt'), 295);
    deepStrictEqual(part2('example.txt'), 1068781);
}

function main() {
    const part1Result = part1('13.txt');
    console.log(`part 1: ${part1Result}`);
    const part2Result = part2('13.txt');
    console.log(`part 2: ${part2Result}`);
}

test();
main();
