use std::fs;

fn pt1( numbers: &[u64], preamble_len: usize ) -> u64 {
    for (i, num) in numbers.iter().enumerate() {
        if i < preamble_len {
            continue;
        }
        let valid = valid_nums( &numbers[i-preamble_len..i] );
        if !valid.contains(num) {
            return *num;
        }
    }
    0
}

fn pt2( numbers: &[u64], vuln: u64 ) -> u64 {

    for (i, num) in numbers.iter().enumerate() {
        let mut sum: u64 = *num;
        let mut j = 1;
        while sum < vuln {
            sum += numbers[i+j];
            if sum == vuln {
                let min = numbers[i..i+j].iter().min().unwrap();
                let max = numbers[i..i+j].iter().max().unwrap();
                return *min + *max;
            }
            j += 1;
        }
    }

    0
}

fn valid_nums( nums: &[u64] ) -> Vec<u64> {
    let mut ret: Vec<u64> = Vec::new();
    for left in nums {
        for right in nums {
            if left == right {
                continue;
            } else {
                ret.push(left + right);
            }
        }
    }
    ret
}

fn main() {
    let file = fs::read_to_string("input").expect("File not found");

    let numbers: Vec<u64> = file.trim().lines().map(|s| s.parse().unwrap()).collect();

    let preamble_len = 25;

    let vuln = pt1( &numbers, preamble_len );
    println!("Part 1: {}", vuln );
    println!("Part 2: {}", pt2( &numbers, vuln ) );
}

