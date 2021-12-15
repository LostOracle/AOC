use std::fs;
use std::env;
use std::fmt;
use std::collections::HashMap;
use std::collections::hash_map::Entry;

struct Rule {
    first: char,
    second: char,
    result: char
}

impl fmt::Display for Rule {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}{} -> {}", self.first, self.second, self.result)?;

        Ok(())
    }
}

type Polymer = Vec<char>;
fn step(polymer: &Polymer, rules: &Vec<Rule>) -> Polymer {
    let mut new_poly = Polymer::new();

    for win in polymer.windows(2) {
        //print!("Window: {}{}", win[0].to_string(), win[1].to_string());
        let rule = rules.iter().find(|x|
            x.first == win[0]
            && x.second == win[1]).unwrap();

        new_poly.push(rule.first);
        new_poly.push(rule.result);
    }
    // Add the final character from the original string
    new_poly.push(polymer[polymer.len()-1]);

    return new_poly;
}

// Part 2 step takes a map
type PolyMap = HashMap<(char, char), u64>;
fn step_part2(polymer: &PolyMap, rules: &Vec<Rule>) -> PolyMap {
    let mut new_poly = PolyMap::new();

    for pair in polymer {
        let rule = rules.iter().find(|x|
            x.first == pair.0.0
            && x.second == pair.0.1).unwrap();

        let first_pair: &mut u64 = match new_poly.entry((rule.first, rule.result)) {
            Entry::Occupied(o) => o.into_mut(),
            Entry::Vacant(v) => v.insert(0),
        };

        *first_pair += pair.1; // Add the current count

        let second_pair: &mut u64 = match new_poly.entry((rule.result, rule.second)) {
            Entry::Occupied(o) => o.into_mut(),
            Entry::Vacant(v) => v.insert(0),
        };

        *second_pair += pair.1; // Add the current count
    }

    return new_poly;
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");

    let mut lines = input.lines();

    // Capture the initial string
    let polymer: Vec<char> = lines.next().unwrap().chars()
        .map(|c| c)
        .collect();

    lines.next(); // Discard a line

    // Capture the rules
    let rules: Vec<Rule> = lines.map(|line| {

        Rule{
            first: line.chars().nth(0).unwrap(),
            second: line.chars().nth(1).unwrap(),
            result: line.chars().nth(line.len()-1).unwrap() }
    }).collect();

    println!("Initial string: \n{}",
        polymer.iter().fold(String::new(), |sum,x| sum + &x.to_string() + ""));
    for rule in &rules {
        println!("{}", rule);
    }

    // Part 1 - Execute rules 10x
    let mut part1_polymer = polymer.clone();
    for i in 0..10 {
        part1_polymer = step(&part1_polymer, &rules);
        //println!("Part 1 ({}):  \n{}", i,
        //    part1_polymer.iter().fold(String::new(), |sum,x| sum + &x.to_string() + ""));
        println!("Part 1 ({}): Len = {}", i, part1_polymer.len());
    }

    // Now calculate what it wants - highest freq minus lowest freq
    let mut freq_map = HashMap::new();
    for c in part1_polymer {
        let letter_count: &mut u64 = match freq_map.entry(c) {
            Entry::Occupied(o) => o.into_mut(),
            Entry::Vacant(v) => v.insert(0),
        };

        *letter_count += 1;
    }

    // Get highest and lowest freq
    let highest_freq = freq_map.values().max().unwrap();
    let lowest_freq = freq_map.values().min().unwrap();
    println!("Part 1: Max - min = {}", highest_freq - lowest_freq);

    // TODO - Part 2 - store all pairs in a map
    // Update the map by generating new pairs based on the rules
    // i.e. each pair spawns 2 new pairs
    // Length of the final string is total # of pairs + 1
    let mut poly_map = HashMap::<(char, char), u64>::new();
    for pair in polymer.windows(2) {
        let map_entry: &mut u64 = match poly_map.entry((pair[0], pair[1])) {
            Entry::Occupied(o) => o.into_mut(),
            Entry::Vacant(v) => v.insert(0),
        };

        *map_entry += 1;
    }

    // Execute part 2
    for _ in 0..40 {
        poly_map = step_part2(&poly_map, &rules);
    }

    // For figuring out the result, we need max freq letter - least freq letter
    // Problem is that putting things in a map loses info about which pair is first
    // We could probably figure this out by keeping track of the first and last letter
    // and doing some intelligent calculation
    // I, however, am simply going to do both calculations and try both answers

    // Count the frequencies of each FIRST letter
    let mut letter_map = HashMap::<char, u64>::new();
    for poly_pair in &poly_map {
        //println!("{}{}: {}", poly_pair.0.0, poly_pair.0.1, poly_pair.1);
        
        let letter: &mut u64 = match letter_map.entry(poly_pair.0.0) {
            Entry::Occupied(o) => o.into_mut(),
            Entry::Vacant(v) => v.insert(0),
        };

        *letter += poly_pair.1; // Add the count of this pair
    }

    //for letter in &letter_map {
    //    println!("{}: {}, ", letter.0, letter.1);
    //}
    let highest_freq = letter_map.values().max().unwrap();
    let lowest_freq = letter_map.values().min().unwrap();
    println!("Part 2a: Max - min = {}", highest_freq - lowest_freq);

    // Count the frequencies of each SECOND letter
    let mut letter_map_2 = HashMap::<char, u64>::new();
    for poly_pair in &poly_map {
        //println!("{}{}: {}", poly_pair.0.0, poly_pair.0.1, poly_pair.1);
        
        let letter: &mut u64 = match letter_map_2.entry(poly_pair.0.1) {
            Entry::Occupied(o) => o.into_mut(),
            Entry::Vacant(v) => v.insert(0),
        };

        *letter += poly_pair.1; // Add the count of this pair
    }

    //for letter in &letter_map_2 {
    //    println!("{}: {}, ", letter.0, letter.1);
    //}
    let highest_freq = letter_map_2.values().max().unwrap();
    let lowest_freq = letter_map_2.values().min().unwrap();
    println!("Part 2b: Max - min = {}", highest_freq - lowest_freq);
}
