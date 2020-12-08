const assert = require('assert');
const { getLinesFromFile } = require('../util/util');

// format
// 5-10 l: glltvgdngllzlllplrlh
function getPasswordForLine(line) {
    const passwordStruct = {
        letter: '',
        maxLetterCount: 0,
        minLetterCount: 0,
        password: '',
    };
    const [countStr, letterStr, password] = line.split(" ");
    passwordStruct.password = password; // password
    const counts = countStr.split("-");
    passwordStruct.minLetterCount = parseInt(counts[0]);
    passwordStruct.maxLetterCount = parseInt(counts[1]);
    passwordStruct.letter = letterStr.substr(0, 1);

    return passwordStruct;
}

function isPasswordValid(passwordStruct) {
    let letterCount = 0;
    const password = passwordStruct.password;
    for(let c of password) {
        if (c == passwordStruct.letter) letterCount++;
    }
    return (letterCount >= passwordStruct.minLetterCount && letterCount <= passwordStruct.maxLetterCount);
}

// reuse same struct but minLetterCount and maxLetterCount now correspond to positions
function isPasswordValidPart2(passwordStruct) {
    const password = passwordStruct.password;
    const position1HasLetter = password.charAt(passwordStruct.minLetterCount - 1) == passwordStruct.letter;
    const position2HasLetter = password.charAt(passwordStruct.maxLetterCount - 1) == passwordStruct.letter;
    return (position1HasLetter || position2HasLetter) && !(position1HasLetter && position2HasLetter);
}

function main() {
    const lines = getLinesFromFile('2.txt');
    const passwords = lines.map(getPasswordForLine);
    const validPasswordCountPart1 = passwords
        .filter(isPasswordValid)
        .length;

    const validPasswordCountPart2 = passwords
        .filter(isPasswordValidPart2)
        .length;

    console.log(`part 1: ${validPasswordCountPart1} valid passwords`);    
    console.log(`part 2: ${validPasswordCountPart2} valid passwords`);    
}

assert.deepStrictEqual(
    getPasswordForLine('5-10 l: glltvgdngllzlllplrlh'),
    {
        letter: 'l',
        minLetterCount: 5,
        maxLetterCount: 10,
        password: 'glltvgdngllzlllplrlh',
    },
    'getPasswordForLine not working'
);

assert.deepStrictEqual(
    isPasswordValid({
        letter: 'l',
        minLetterCount: 5,
        maxLetterCount: 10,
        password: 'glltvgdngllzlllplrlh',
    }),
    true,
    "isPasswordValid failed on test 1"
)

assert.deepStrictEqual(
    isPasswordValid({
        letter: 'w',
        minLetterCount: 1,
        maxLetterCount: 2,
        password: 'wzww',
    }),
    false,
    "isPasswordValid failed on test 2"
)

assert.deepStrictEqual(
    isPasswordValidPart2(getPasswordForLine('1-3 a: abcde')),
    true,
    'isPasswordValidPart2 failed test 1',
);

assert.deepStrictEqual(
    isPasswordValidPart2(getPasswordForLine('1-3 b: cdefg')),
    false,
    'isPasswordValidPart2 failed test 2',
);

assert.deepStrictEqual(
    isPasswordValidPart2(getPasswordForLine('2-9 c: ccccccccc')),
    false,
    'isPasswordValidPart2 failed test 3',
);

main();
