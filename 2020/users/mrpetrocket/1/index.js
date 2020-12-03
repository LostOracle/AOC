const fs = require('fs');
const numbers = fs.readFileSync('input.txt', {encoding:'UTF-8'})
    .split('\r\n')
    .map(value => parseInt(value));
for(const num1 of numbers) {
    for(const num2 of numbers) {
        for(const num3 of numbers) {
            if (num1 + num2 + num3 == 2020) {
                console.log(`${num1} + ${num2} + ${num3} = 2020`);
                console.log(`${num1} * ${num2} * ${num3} = ${num1 * num2 * num3}`);
            }
        }
    }
}
