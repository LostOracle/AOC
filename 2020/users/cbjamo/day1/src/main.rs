use std::fs::File;
use std::io::BufReader;
use std::io::prelude::*;
use std::path::Path;
use std::prelude::*;

fn day1( values: &Vec<i32> ) -> i32 {
    let mut ret = 0;
    for val in values {
        let target = 2020 - val;

        if values.binary_search( &target ).is_ok() {
            ret = val * target;
            break;
        }
    }

    ret
}

fn day2( values: &Vec<i32> ) -> i32 {
    let mut ret = 0;

    for left in values {
        for right in values {
            let target = 2020 - ( left + right );

            if values.binary_search( &target ).is_ok() {
                ret = target * left * right;
                break;
            }
        }
    }

    ret
}

fn main() {
    let path = Path::new("input");
    //let display = path.
    
    let mut file = match File::open(&path) {
        Err(why) => panic!("Couldn't p[en {}: {}", path.display(), why),
        Ok(file) => file,
    };

    let mut values = Vec::new();

    for line in BufReader::new(file).lines() {
        let num = line.unwrap().parse::<i32>().unwrap();
        values.push(num);
    }

    values.sort();

    println!( "{}", day1(&values) );
    println!( "{}", day2(&values) );
}

