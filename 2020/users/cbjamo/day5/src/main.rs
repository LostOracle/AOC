use std::fs;
use std::str::FromStr;
use std::convert::TryInto;

#[derive (Debug)]
struct Seat {
    row: u32,
    col: u32,
    id: u32,
}

impl FromStr for Seat {
    type Err = ();

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let row_tree = &s[0..7];
        let col_tree = &s[7..10];

        let row = intfromtree(row_tree);
        let col = intfromtree(col_tree);

        let id = row * 8 + col;
        Ok( Self{ row, col, id } )
    }
}

fn intfromtree (s: &str) -> u32 {
    let mut accum = 0;
    for (i, c) in s.chars().rev().enumerate() {
        if c == 'B' || c == 'R' {
            accum += 2_u32.pow( i.try_into().unwrap());
        }
    }

    accum
}

use std::collections::HashSet;

fn main() {
    let file = fs::read_to_string("input").expect("File not found");

    let values = file.trim().lines().map( intfromtree );

    println!("pt1: {}", values.clone().max().unwrap());

    let taken: HashSet<_> = values.clone().collect();
    let seat = values.clone().find_map(|i| {
        if !taken.contains( &(i+1) ) && taken.contains( &(i+2) ) {
            Some(i+1)
        } else {
            None
        }
    }).unwrap();
    println!("pt2: {}", seat);
}

