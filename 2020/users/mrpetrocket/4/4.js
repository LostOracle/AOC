const assert = require('assert');
const fs = require('fs');
const { getLinesFromFile } = require('../util/util');

const passportValidation = require('./passport-validation');

function getPassports(lines) {
    const passports = [];
    let currentPassport = {};
    for(line of lines) {
        if (line == "") {
            passports.push(currentPassport);
            currentPassport = {};
        } else {
            const properties = line.split(" ");
            for(property of properties) {
                const [key, value] = property.split(":");
                currentPassport[key] = value;
            }
        }
    }
    passports.push(currentPassport);
    return passports;
}

function isPassportValidPart1(passport) {
    // everything but 'cid'
    const requiredFields = ['byr', 'iyr', 'eyr', 'hgt', 'hcl', 'ecl', 'pid'];
    for(field of requiredFields) {
        if (!passport.hasOwnProperty(field)) return false;
    }
    return true;
}

function test() {
    const exampleLines = getLinesFromFile('example.txt');
    assert.deepStrictEqual(
        getPassports(exampleLines),
        JSON.parse(fs.readFileSync('./example_passports.json', { encoding: 'UTF-8' })),
    );
    const validPassportsPart1 = getPassports(exampleLines)
        .filter(isPassportValidPart1);
    assert.strictEqual(validPassportsPart1.length, 2);

    passportValidation.test();

    const invalidPassportLines = getLinesFromFile('invalidpassports.txt');
    assert.deepStrictEqual(
        getPassports(invalidPassportLines)
            .filter(isPassportValidPart1)
            .filter(passportValidation.isPassportValid)
            .length,
        0
    );

    const validPassportLines = getLinesFromFile('validpassports.txt');
    assert.deepStrictEqual(
        getPassports(validPassportLines)
            .filter(isPassportValidPart1)
            .filter(passportValidation.isPassportValid)
            .length,
        4
    );
}

function main() {
    const lines = getLinesFromFile('4.txt');
    const validPassportsPart1 = getPassports(lines)
        .filter(isPassportValidPart1);
    console.log(`part 1: ${validPassportsPart1.length} valid passports`);
    const validPassportsPart2 = validPassportsPart1.filter(passportValidation.isPassportValid);
    console.log(`part 2: ${validPassportsPart2.length} valid passports`);    
}

test();
main();
