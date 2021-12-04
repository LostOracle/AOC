pub type Input = (usize, Vec<u64>);

#[aoc_generator(day3)]
pub fn input_gen(input: &str) -> Input {
    let bits = input.lines().nth(0).unwrap().len();
    let nums = input.lines().map(|l| {
        u64::from_str_radix(l, 2).unwrap()
    }).collect();
    (bits, nums)
}

fn get_bit(number: &u64, bit: usize) -> bool {
    number & (1 << bit) != 0
}

#[aoc(day3, part1)]
pub fn part1(input: &Input) -> u64 {
    let bits = input.0;
    let nums = &input.1;
    let mut ones: Vec<usize> = Vec::new();
    ones.resize(bits,0);

    for num in nums {
        for i in 0..bits {
            if get_bit(num, i) {
                ones[i] += 1;
            }
        }
    }

    let mut gam: u64 = 0;
    for i in 0..bits {
        if ones[i] > (nums.len() / 2) {
            gam |= 1<<i;
        }
    }
    let eps = (!gam<<(64-bits))>>(64-bits);

    gam * eps
}

#[aoc(day3, part2)]
pub fn part2(input: &Input) -> u64 {
    
}

#[cfg(test)]
mod tests {
    use super::*;


    #[test]
    fn testpart1() {
    let input = "00100
11110
10110
10111
10101
01111
00111
11100
10000
11001
00010
01010";
        assert_eq!(part1(&input_gen(input)), 198);

    }

    #[test]
    fn testpart2() {
    let input = "00100
11110
10110
10111
10101
01111
00111
11100
10000
11001
00010
01010";
        assert_eq!(part2(&input_gen(input)), 230);

    }
}
