const { getLinesFromFile } = require('../util/util');

function ski(xSlope, ySlope) {
    const lines = getLinesFromFile('3.txt');
    const sectionWidth = lines[0].length;
    const mapHeight = lines.length;
    let x = 0;
    let y = 0;
    let treeCount = 0;
    while(y < mapHeight) {
        const xInSection = x % sectionWidth;
        if (lines[y][xInSection] == '#') treeCount++;
        x += xSlope;
        y += ySlope;
    }
    return treeCount;
}

const part1Trees = ski(3, 1);
console.log(`hit ${part1Trees} trees`);

const part2 = ski(1, 1) * ski(3, 1) * ski(5, 1) * ski(7, 1) * ski(1, 2);
console.log(`part 2: ${part2}`);


