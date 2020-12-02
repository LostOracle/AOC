"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const promises_1 = require("fs/promises");
const path_1 = require("path");
async function main() {
    let input_file_path = null;
    for (let i = 2; i < process.argv.length; ++i) {
        input_file_path = process.argv[i];
    }
    if (!input_file_path) {
        input_file_path = path_1.resolve(__dirname, "input.txt");
    }
    try {
        let stats = await promises_1.stat(input_file_path);
        if (!stats.isFile()) {
            console.log("given input path is not a file");
            return 1;
        }
    }
    catch (err) {
        if (err.code === "ENOENT") {
            console.log("given input file does not exist");
            return 1;
        }
        else {
            throw err;
        }
    }
    let contents = await promises_1.readFile(input_file_path, { encoding: "utf-8" });
    let number_set = new Set();
    for (let num_str of contents.split("\n")) {
        number_set.add(parseInt(num_str));
    }
    let primary = 0;
    let secondary = 0;
    for (let num of number_set) {
        let check = 2020 - num;
        if (number_set.has(check)) {
            primary = num;
            secondary = check;
            break;
        }
    }
    console.log(primary * secondary);
    return 0;
}
main().catch(err => {
    console.log("uncaught error", err.stack);
    return 1;
}).then(code => {
    process.exit(code);
});
