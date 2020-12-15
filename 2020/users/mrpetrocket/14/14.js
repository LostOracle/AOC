const { getLinesFromFile } = require('../util/util');
const { deepStrictEqual } = require('assert');

const DECODER_VERSION_1 = 1, DECODER_VERSION_2 = 2;

// commands have a regex and a parse function to turn regex results into a command
const availableCommands = [{
    parse(results) {
        return createCommand('mem', {
            address: parseInt(results[1]),
            value: parseInt(results[2]),
        });
    },
    regex: /^mem\[(\d+)\] = (\d+)$/,
}, {
    parse(results) {
        return createCommand('mask', {
            mask: results[1],
        });
    },
    regex: /^mask = ([X01]+)$/,
}];

/**
 * mask = string of 01X, value = numeric value
 * returns numeric value
 */
function applyMaskToValue(mask, value) {
    let valueStr = value.toString(2);
    if (valueStr.length < mask.length) {
        // pad value to be equal to mask.
        valueStr = leftPad(valueStr, '0', mask.length);
    }

    // mask value according to masking rules
    // X means use the original value bits
    // 0 or 1 means use the mask bits instead of the value bits
    const maskedBinary = valueStr
        .split('')
        .map((valueChar, index) => {
            if (mask[index] === 'X') return valueChar;
            return mask[index];
        })
        .join('');
    return parseInt(maskedBinary, 2);
}

/**
 * for decoder version 2, apply a mask to an address
 * mask char meanings
 * 0 - unchanged
 * 1 - overwritten with 1
 * X - overwritten with X, this matters when we save to the address
 * return address as a binary string with the "X"'s intact
 */
function applyMaskToMemoryAddress(mask, address) {
    let valueStr = address.toString(2);
    if (valueStr.length < mask.length) {
        valueStr = leftPad(valueStr, '0', mask.length);
    }

    return valueStr
        .split('')
        .map((valueChar, index) => {
            if (mask[index] === '0') return valueChar;
            return mask[index];
        })
        .join('');
}

function createCommand(name, arguments) {
    return {
        arguments,
        name,
    };
}

function createHardware(decoder) {
    return {
        decoder,
    };
}

function createMemoryAddress(address, value) {
    return {
        address,
        value,
    };
}

function getCommandFromLine(line) {
    for(command of availableCommands) {
        const test = command.regex.exec(line);
        if (test) {
            return command.parse(test)
        }
    }
}

function getCommandsFromFile(filename) {
    return getLinesFromFile(filename).map(getCommandFromLine);
}

function leftPad(s, char, paddedLength) {
    while(s.length < paddedLength) {
        s = char + s;
    }
    return s;
}

function part1(filename) {
    return runAndSum(filename, DECODER_VERSION_1);
}

function part2(filename) {
    return runAndSum(filename, DECODER_VERSION_2);
}

/**
 * Recurse through masked address and create permutations for each X wildcard
 * 
 * Any 'X' bit in the mask can be 0 or 1, so we return every possible permutation of the masked address
 * 
 * iterate through the maskedAddress string starting at index
 * if we encounter an X, drill down as if X was 0 or 1
 * if we encounter no X's, return the address - no more recursion on this path
 */
function recursivelyGetMemoryAddresses(
    maskedAddress,
    index,
) {
    if (index === undefined) index = 0;
    const nextWildcardIndex = maskedAddress.indexOf('X', index);
    if (nextWildcardIndex === -1) {
        // no more wildcards; return this address
        return [ maskedAddress ];
    } else {
        // found wildcard - wildcard can be 0 or 1
        // drill down to the next layer
        return [0, 1]
            .map(char => recursivelyGetMemoryAddresses(
                maskedAddress.substring(0, nextWildcardIndex) + char + maskedAddress.substring(nextWildcardIndex + 1),
                index + 1
            ))
            .flat();
    }
}

function runAndSum(filename, decoder) {
    const commands = getCommandsFromFile(filename);
    const mem = runProgram(commands, createHardware(decoder));
    return sumElementsInMemory(mem);
}

/**
 * run a list of commands
 * returns memory state when program finishes
 */
function runProgram(commands, hardware) {
    const mem = [];
    let mask = '';
    for(command of commands) {
        if (command.name === 'mask') mask = command.arguments.mask;
        else if (command.name === 'mem') {
            if (hardware.decoder === DECODER_VERSION_1) {
                const value = applyMaskToValue(mask, command.arguments.value);
                setMemoryLocation(mem, command.arguments.address, value);

            } else if (hardware.decoder === DECODER_VERSION_2) {
                const maskedAddress = applyMaskToMemoryAddress(mask, command.arguments.address);
                setMemoryLocationWithDecoderVersionDos(mem, maskedAddress, command.arguments.value);

            } else throw new Error(`Unsupported decoder version ${hardware.decoder}`);
        }
    }
    return mem;
}

/**
 * Sets value in memory
 * Either overwrites existing value at that address or makes a new value at that address
 * Mutates memory array
 */
function setMemoryLocation(memory, address, value) {
    const existingLocation = memory.find(memLocation => memLocation.address === address);
    if (existingLocation) existingLocation.value = value;
    else memory.push(createMemoryAddress(address, value));
}

/**
 * Set a memory location by emulating the behavior of Decoder Chip V2
 */
function setMemoryLocationWithDecoderVersionDos(
    memory,
    maskedAddress,
    value
) {
    const addresses = recursivelyGetMemoryAddresses(maskedAddress);
    addresses.forEach(address => {
        const memoryAddress = parseInt(address, 2);
        setMemoryLocation(memory, memoryAddress, value);
    });
}

function sumElementsInMemory(memory) {
    return memory.reduce((total, memoryElement) => total + memoryElement.value, 0);
}

function test() {
    deepStrictEqual(applyMaskToValue('XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X', 11), 73);
    deepStrictEqual(part1('example.txt'), 165);
    deepStrictEqual(applyMaskToMemoryAddress(
        '00000000000000000000000000000000X0XX',
        '000000000000000000000000000000011010',
    ), '00000000000000000000000000000001X0XX');
    deepStrictEqual(
        recursivelyGetMemoryAddresses('00000000000000000000000000000001X0XX'),
        [
            '000000000000000000000000000000010000',
            '000000000000000000000000000000010001',
            '000000000000000000000000000000010010',
            '000000000000000000000000000000010011',
            '000000000000000000000000000000011000',
            '000000000000000000000000000000011001',
            '000000000000000000000000000000011010',
            '000000000000000000000000000000011011',
        ],
    );
    deepStrictEqual(part2('example2.txt'), 208);
}

function main() {
    const part1MemorySum = part1('14.txt');
    console.log(`Part 1: ${part1MemorySum}`);
    const part2MemorySum = part2('14.txt');
    console.log(`Part 2: ${part2MemorySum}`);
}

test();
main();
