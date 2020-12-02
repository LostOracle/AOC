use std::fs::File;
use std::io::BufReader;
use std::io::prelude::*;
use std::path::Path;
#[macro_use] extern crate scan_fmt;

fn day1( values: &Vec<Password> ) -> i32 {
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

fn day2( values: &Vec<Password> ) -> i32 {
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

fn main() {
    let path = Path::new("input");
    //let display = path.
    
    let file = match File::open(&path) {
        Err(why) => panic!("Couldn't open {}: {}", path.display(), why),
        Ok(file) => file,
    };

    let mut values = Vec::new();

    for line in BufReader::new(file).lines() {
        let line = line.unwrap();

        let (min, max, letter, pass) = scan_fmt!(&line, "{d}-{d} {}: {}", usize, usize, char, String).unwrap();
        let password = Password{ min, max, letter, pass };

        values.push(password);
    }

    println!( "{}", day1(&values) );
    println!( "{}", day2(&values) );
}

