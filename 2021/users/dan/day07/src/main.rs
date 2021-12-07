use std::fs;
use std::env;

// Calculates the triangular # for a given input
fn triangular(val: i64) -> i64 {
    val * (val + 1) / 2
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");

    let crabbies: Vec<i64> = input.lines().next().unwrap()
        .split(',')
        .map(|x| x.parse::<i64>().unwrap())
        .collect();


    let max_crab: i64 = *crabbies.iter().max().unwrap();

    // Part 1 - costs are linear
    let max_cost = max_crab * max_crab;
    let min_cost = (0..max_crab)
        .fold(max_cost, |best,pos| { 
            let cost = crabbies.iter()
                .map(|c| (c-pos).abs())
                .sum();

            if cost < best { cost }
            else { best }
        });

    println!("Part1: Min cost: {}, max cost: {}", min_cost, max_cost);


    // Part 2 - costs are triangular
    let max_cost = i64::pow(triangular(max_crab), 2);
    let min_cost = (0..max_crab)
        .fold(max_cost, |best,pos| { 
            let cost = crabbies.iter()
                .map(|c| triangular((c-pos).abs()))
                .sum();

            if cost < best { cost }
            else { best }
        });

    println!("Part2: Min cost: {}, max cost: {}", min_cost, max_cost);

}
