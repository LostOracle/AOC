const { deepStrictEqual } = require('assert');
const { getLinesFromFile } = require('../util/util');

const EXIT_NORMAL = 1;
const EXIT_INFINITE_LOOP = 0;

/**
 * transform file line to an instruction type
 * example line: "acc +49"
 */
function lineToInstruction(line) {
    const [
        throwaway,
        command,
        argument
    ] = /([a-z]+)\s\+?(-?\d+)/.exec(line);
    return createInstruction(command, parseInt(argument));
}

function createInstruction(command, argument) {
    return {
        argument,
        command,
    };
}

/**
 * Run an instruction within the execution context.
 * Return a new execution context showing how the instruction
 * modified the context.
 */
function executeInstruction(
    instruction,
    executionContext
) {
    const { accumulator, pointer } = executionContext;
    switch(instruction.command) {
        case 'acc':
            return createExecutionContext(
                pointer + 1,
                accumulator + instruction.argument,
            );
        case 'jmp':
            return createExecutionContext(
                pointer + instruction.argument,
                accumulator,
            )
        case 'nop':
            return createExecutionContext(
                pointer + 1,
                accumulator
            );
        default:
            throw new Error(`unknown command "${instruction.command}"`);
    }
}

function getInstructionsFromFile(filename) {
    return getLinesFromFile(filename).map(lineToInstruction);
}

/**
 * wrap runProgram to read a program from a file
 */
function runProgramFromFile(filename) {
    return runProgram(getInstructionsFromFile(filename));
}

/**
 * run a list of instructions; return the execution context
 * directly before the first time we would run
 * an instruction twice
 */
function runProgram(rawInstructions) {
    let executionContext = createExecutionContext(0, 0);
    const instructions = rawInstructions.map(instruction => ({
        executed: false,
        ...instruction,
    }));
    while(executionContext.pointer < instructions.length) {
        const hasCurrentInstructionExecuted = instructions[executionContext.pointer]['executed'];
        if (hasCurrentInstructionExecuted) {
            // we are about to execute an instruction twice
            // which means we have an infinite loop
            return createProgramResult(EXIT_INFINITE_LOOP, executionContext);
        } else {
            instructions[executionContext.pointer]['executed'] = true;            
        }
        executionContext = executeInstruction(
            instructions[executionContext.pointer],
            executionContext
        );
    }
    return createProgramResult(EXIT_NORMAL, executionContext);
}

/**
 * flip every jmp->nop and nop->jmp until we find
 * a combination that terminates normally
 * returns results on the fix
 * if this function was more general purpose, I could do a lot less work.
 */
function fixProgramFromFile(filename) {
    const instructions = getInstructionsFromFile(filename);
    for(let i=0; i<instructions.length; i++) {
        const instruction = instructions[i];
        // acc instructions do not flip
        if (instruction.command === 'acc') continue;

        flipInstruction(instruction);
        const result = runProgram(instructions);
        flipInstruction(instruction);
        if (result.exitCode === EXIT_NORMAL) {
            return {
                executionContext: result.executionContext,
                fixedInstruction: instruction,
                fixedInstructionIndex: i,
            };
        }
    }
    throw new Error('fixProgramFromFile did not find a flip to fix the program');
}

/**
 * flips jmp->nop and nop->jmp
 */
function flipInstruction(instruction) {
    if (instruction.command === 'jmp')
        instruction.command = 'nop';
    else
        instruction.command = 'jmp';
}

function createExecutionContext(pointer, accumulator) {
    return {
        accumulator,
        pointer,
    };
}

function createProgramResult(exitCode, executionContext) {
    return {
        executionContext,
        exitCode,
    };
}

function test() {
    const instruction1 = lineToInstruction('acc +49');
    const instruction2 = lineToInstruction('jmp -110');

    deepStrictEqual(instruction1, {command:'acc', argument: 49});
    deepStrictEqual(instruction2, {command:'jmp', argument: -110});

    const initialExecutionContext = createExecutionContext(0, 0);
    deepStrictEqual(executeInstruction(instruction1, initialExecutionContext), {accumulator: 49, pointer: 1});
    deepStrictEqual(executeInstruction(instruction2, initialExecutionContext), {accumulator: 0, pointer: -110});

    const result = runProgramFromFile('example.txt');
    deepStrictEqual(result.exitCode, EXIT_INFINITE_LOOP);
    deepStrictEqual(result.executionContext.accumulator, 5);

    const fixResult = fixProgramFromFile('example.txt');
    deepStrictEqual(fixResult.executionContext.accumulator, 8);
    deepStrictEqual(fixResult.fixedInstructionIndex, 7);
}

function main() {
    const part1Result = runProgramFromFile('8.txt');
    console.log(`Part 1: Accumulator reads ${part1Result.executionContext.accumulator} before the infinite loop started.`);
    const part2Result = fixProgramFromFile('8.txt');
    console.log(`Part 2: Accumulator reads ${part2Result.executionContext.accumulator} after normal termination.`);
    console.log(`        To fix the program, we needed to flip the instruction at line ${part2Result.fixedInstructionIndex + 1}.`);
}

test();
main();