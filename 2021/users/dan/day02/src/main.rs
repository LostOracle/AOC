use std::fs;
use std::fmt;
use std::env;

struct Pos {
    hor: u64,
    ver: u64,
    aim: u64,
}

impl fmt::Display for Pos {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "({}, {}, {})", self.hor, self.ver, self.aim)
    }
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");

    let mut pos = Pos { hor:0, ver:0, aim:0 };

    // For each cmd, calculate resulting pos
    for cmd in input.lines() {
        let dir = cmd.chars().nth(0).unwrap();
        let quantity = cmd.split_whitespace().last().unwrap().parse::<u64>()
            .expect("Unable to parse quantity as an int");
        match dir {
            'f' => pos.hor += quantity,
            'd' => pos.ver += quantity,
            'u' => { 
                if quantity <= pos.ver { pos.ver -= quantity }
                else{ pos.ver = 0 }
            }
            _ => { println!("Unrecognized cmd: {}", cmd); panic!(); },
        }
    }

    println!("Part 1: {} => {}", pos, pos.hor * pos.ver);

    let mut pos = Pos{ hor:0, ver:0, aim:0 };

    // Part 2
    // For each cmd, calculate resulting pos
    for cmd in input.lines() {
        let dir = cmd.chars().nth(0).unwrap();
        let quantity = cmd.split_whitespace().last().unwrap().parse::<u64>()
            .expect("Unable to parse quantity as an int");
        match dir {
            'f' => {
                pos.ver += pos.aim * quantity;
                pos.hor += quantity;
            },
            'd' => {
                pos.aim += quantity;
            },
            'u' => { 
                pos.aim -= quantity;
            }
            _ => { println!("Unrecognized cmd: {}", cmd); panic!(); },
        }
    }

    println!("Part 2: {} => {}", pos, pos.hor * pos.ver);
}
