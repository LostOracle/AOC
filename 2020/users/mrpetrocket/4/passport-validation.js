const assert = require('assert');

const validationSchema = {
    'byr': yearValidator(1920, 2002),
    'iyr': yearValidator(2010, 2020),
    'eyr': yearValidator(2020, 2030),
    'hgt': heightValidator,
    'hcl': hairColorValidator,
    'ecl': eyeColorValidator,
    'pid': passportIdValidator
}

/**
 * pid (Passport ID) - a nine-digit number, including leading zeroes.
 */
function passportIdValidator(val) {
    return regexMatcher(/^\d{9}$/)(val);
}

/**
 * ecl (Eye Color) - exactly one of: amb blu brn gry grn hzl oth.
 */
function eyeColorValidator(val) {
    return ['amb', 'blu', 'brn', 'gry', 'grn', 'hzl', 'oth'].includes(val);
}

/**
 * hcl (Hair Color) - a # followed by exactly six characters 0-9 or a-f.
 */
function hairColorValidator(val) {
    return regexMatcher(/^#[0-9a-f]{6}$/)(val);
}

function regexMatcher(regex) {
    return function(val) {
        return regex.test(val);
    }
} 

/**
 * hgt (Height) - a number followed by either cm or in:
 * If cm, the number must be at least 150 and at most 193.
 * If in, the number must be at least 59 and at most 76.
 */
function heightValidator(val) {
    let isValid = true;
    if (!regexMatcher(/^\d{1,}(in|cm)$/)(val)) isValid = false;
    const height = parseInt(val.substring(0, val.length-2));
    const unit = val.substring(val.length - 2);
    if (unit === 'in' && !inRangeInclusive(height, 59, 76)) isValid = false;
    if (unit === 'cm' && !inRangeInclusive(height, 150, 193)) isValid = false;
    return isValid;
}

function inRangeInclusive(val, min, max) {
    return (val >= min && val <= max);
}

/**
 *  * byr (Birth Year) - four digits; at least 1920 and at most 2002.
iyr (Issue Year) - four digits; at least 2010 and at most 2020.
eyr (Expiration Year) - four digits; at least 2020 and at most 2030.
 */
function yearValidator(min, max) {
    return function(val) {
        if (!regexMatcher(/^\d{4}$/)(val)) return false;
        if (!inRangeInclusive(parseInt(val), min, max)) return false;
        return true;
    }
}

function isPassportValid(passport) {
    for(field in validationSchema) {
        if (!validationSchema[field](passport[field])) {
            return false;
        }
    }
    return true;
}

function test() {
    assert.strictEqual(yearValidator(2020, 2030)(2022), true);
    assert.strictEqual(yearValidator(2020, 2030)(1920), false);
}

module.exports = {
    isPassportValid,
    test,
};
