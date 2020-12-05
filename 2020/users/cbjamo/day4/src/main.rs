use std::str::FromStr;
#[macro_use] extern crate scan_fmt;

fn part1( values: &Vec<ID> ) -> i32 {
    let mut ret = 0;
    for val in values {
        if val.validate() {
            println!("val {:?}", val);
            ret += 1;
        } else {
            println!("inv {:?}", val);
        }
    }

    ret
}

#[derive (Debug)]
struct ID {
    byr: Option<i32>,
    iyr: Option<i32>,
    eyr: Option<i32>,
    hgt: Option<String>,
    hcl: Option<String>,
    ecl: Option<String>,
    pid: Option<String>,
    cid: Option<String>,
}

extern crate regex;
use regex::Regex;

impl ID {
    fn new() -> ID {
        ID {
            byr: None,
            iyr: None,
            eyr: None,
            hgt: None,
            hcl: None,
            ecl: None,
            pid: None,
            cid: None,
        }
    }

    fn validate(&self) -> bool {
        // Check first if we are missing data
        if !(self.byr.is_some() && self.iyr.is_some() && self.eyr.is_some() && self.hgt.is_some() && self.hcl.is_some() && self.ecl.is_some() && self.pid.is_some()) {
            return false;
        }

        if self.byr < Some(1920) || self.byr > Some(2002) { return false; }
        if self.iyr < Some(2010) || self.iyr > Some(2020) { return false; }
        if self.eyr < Some(2020) || self.eyr > Some(2030) { return false; }

        let hgt = self.hgt.as_ref().unwrap();
        if hgt.contains("in") {
            let inch = self.hgt.as_ref().unwrap().strip_suffix("in").unwrap().parse::<i32>().unwrap();
            if inch > 76 || inch < 59 {
                return false;
            }
        } else if hgt.contains("cm") {
            let cm = self.hgt.as_ref().unwrap().strip_suffix("cm").unwrap().parse::<i32>().unwrap();
            if cm > 193 || cm < 150 {
                return false;
            }
        } else {
            return false;
        }

        let hcl = self.hcl.as_ref().unwrap();
        let re = Regex::new(r"^#[0-9a-f]{6}$").unwrap();
        if !re.is_match(hcl) {
            return false;
        }

        let valid_eye = ["amb", "blu", "brn", "gry", "grn", "hzl", "oth"];
        let ecl = self.ecl.as_ref().unwrap().as_str();
        if !valid_eye.contains(&ecl) || ecl.len() != 3 {
            return false;
        }

        let pid = self.pid.as_ref().unwrap();
        let re = Regex::new(r"^[0-9]{9}$").unwrap();
        if !re.is_match(pid) {
            return false;
        }

        return true;
    }
}

impl FromStr for ID {
    type Err = ();

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut ret: ID = ID::new();
        let toks = s.split(" ");
        for tok in toks {
            if tok.len() == 0 {
                break;
            }
            let (key, value) = scan_fmt!(tok, "{}:{}", String, String).unwrap();
            match key.as_str() {
                "byr" => ret.byr = Some(value.parse().unwrap()),
                "iyr" => ret.iyr = Some(value.parse().unwrap()), 
                "eyr" => ret.eyr = Some(value.parse().unwrap()),
                "hgt" => ret.hgt = Some(value),
                "hcl" => ret.hcl = Some(value),
                "ecl" => ret.ecl = Some(value),
                "pid" => ret.pid = Some(value),
                "cid" => ret.cid = Some(value),
                _ => println!("BAD {} {}", key, value),
            }
        }

        Ok( ret )
    }
}

use std::fs::File;
use std::io::{BufRead, BufReader};

fn main() {
    let file = File::open("input").unwrap();
    let file = BufReader::new(file);

    let mut values: Vec<ID> = Vec::new();

    let mut s = "".to_string();
    for line in file.lines() {
        let line = line.unwrap();
        if line.len() > 0 {
            s += &line;
            s += " ";
        }
        else {
            values.push( s.parse().unwrap() );
            s.clear();
        }
    }
    values.push( s.parse().unwrap() );


    println!( "Part 1: {}", part1(&values) );
}

