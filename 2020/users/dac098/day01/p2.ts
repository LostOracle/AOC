import { readFile, stat } from "fs/promises"
import { resolve } from "path";

async function main() {
    let input_file_path = null;

    for (let i = 2; i < process.argv.length; ++i) {
        input_file_path = process.argv[i];
    }

    if (!input_file_path) {
        input_file_path = resolve(__dirname, "input.txt");
    }

    try {
        let stats = await stat(input_file_path);

        if (!stats.isFile()) {
            console.log("given input path is not a file");
            return 1;
        }
    } catch (err) {
        if (err.code === "ENOENT") {
            console.log("given input file does not exist");
            return 1;
        } else {
            throw err;
        }
    }

    let contents = await readFile(input_file_path, {encoding: "utf-8"});
    let number_set = new Set<number>();

    for (let num_str of contents.split("\n")) {
        number_set.add(parseInt(num_str));
    }

    let tuple = [];

    for (let num of number_set) {
        let check = 2020 - num;

        for (let num2 of number_set) {
            if (num2 === num) {
                continue;
            }

            let check2 = check - num2;

            if (number_set.has(check2)) {
                tuple = [num, num2, check2];
                break;
            }
        }
    }

    if (tuple.length) {
        let total = tuple[0] * tuple[1] * tuple[2];
        console.log(total);
    } else {
        console.log("failed to find match");
    }

    return 0;
}

main().catch(err => {
    console.log("uncaught error", err.stack);
    return 1;
}).then(code => {
    process.exit(code);
});