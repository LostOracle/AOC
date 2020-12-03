use std::fs;
use std::str::FromStr;
#[macro_use] extern crate scan_fmt;

fn part1( values: &Vec<Password> ) -> i32 {
    let mut ret = 0;
    for val in values {
        let count = val.pass.matches(val.letter).count();

        //println!("count: {}, {:?}", count, val);

        if (count >= val.min) && (count <= val.max) {
            ret += 1;
        }
    }

    ret
}

fn part2( values: &Vec<Password> ) -> i32 {
    let mut ret = 0;
    for val in values {
        if (val.pass.chars().nth(val.min-1).unwrap() == val.letter) ^ (val.pass.chars().nth(val.max-1).unwrap() == val.letter) {
            ret += 1;
        }
    }

    ret
}

#[derive (Debug)]
struct Password {
    min: usize,
    max: usize,
    letter: char,
    pass: String
}

impl FromStr for Password {
    type Err = ();

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let (min, max, letter, pass) = scan_fmt!(s, "{d}-{d} {}: {}", usize, usize, char, String).unwrap();

        Ok(Self{ min, max, letter, pass } )
    }
}

fn main() {
    let file = fs::read_to_string("input").expect("File not found");

    let values: Vec<Password> = file.trim().lines().map(|s| s.parse().unwrap()).collect();

    println!( "Part 1: {}", part1(&values) );
    println!( "Part 2: {}", part2(&values) );
}

