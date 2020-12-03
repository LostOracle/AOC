use std::fs;
use std::str::FromStr;
#[macro_use] extern crate scan_fmt;

#[derive (Debug)]
struct Slope {
    right: usize,
    down: usize,
}

fn main() {
    let file = fs::read_to_string("input").expect("File not found");

    let field: Vec<Vec<bool>> = file.trim().lines().map(|s| s.chars().map(|c| if c == '#' { return true } else { return false } ).collect()).collect();

    let slopes = [Slope{right:1, down:1}, Slope{right:3, down:1}, Slope{right:5, down:1}, Slope{right:7, down:1}, Slope{right:1, down:2}];

    let mut ans = Vec::new();

    for slope in slopes.iter() {
        println!("{:?}", slope);
        let mut trees = 0;

        let mut x = 0;
        let mut y = 0;

        loop {
            if field[y][x] {
                trees += 1;
            }
            x = (x + slope.right) % field[0].len();
            y = y + slope.down;
            if y >= field.len() {
                break;
            }
        }

        ans.push(trees);
    }

    println!("{:?}", ans );
    println!("{}", ans.iter().product::<i64>() );
}

