const readline = require("readline");
const fs = require("fs");
const path = require("path");

function calcFuel(integer) {
    return Math.floor(integer / 3) - 2;
}

async function main() {
    const input_stream = fs.createReadStream(path.join(__dirname,"./input.txt"));

    const rl = readline.createInterface({
        input: input_stream,
        crlfDelay: Infinity
    });

    let total = 0;
    
    for await (const line of rl) {
        let calc = calcFuel(parseInt(line));

        while (calc > 0) {
            total += calc;
            calc = calcFuel(calc);
        }
    }

    console.log(total);
}

main().catch(console.error);
