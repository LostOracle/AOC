#[derive(PartialEq)]
enum Common {
    Zero,
    One,
    Equal
}

#[aoc_generator(day3)]
pub fn input_gen(input: &str) -> Vec<Vec<bool>> {
    input.lines().map(|l| {
        l.chars().rev().filter_map(|c| {
            match c
            {
                '0' => Ok(false),
                '1' => Ok(true),
                _ => Err("bad char")
            }.ok()
        }).collect()
    }).collect()
}

fn most_common(vec: &Vec<bool>) -> Common {
    let mut ones = 0;
    let mut zeros = 0;
    for i in vec{
        if *i {
            ones += 1;
        } else {
            zeros += 1;
        }
    }
    if ones > zeros {
        Common::One
    } else if zeros > ones {
        Common::Zero
    } else {
        Common::Equal
    }
}

#[aoc(day3, part1)]
pub fn part1(input: &Vec<Vec<bool>>) -> u64 {
    let bits = input[0].len();
    let mut gam = 0;
    let mut eps = 0;

    for bit in 0..bits {
        let column: Vec<bool> = input.iter().map(|num| num[bit]).collect();
        let most_common = match most_common(&column) {
            Common::One => true,
            Common::Zero => false,
            Common::Equal => false
        };
        gam |= (most_common as u64) << bit;
        eps |= (!most_common as u64) << bit;
    }

    gam * eps
}

fn vb_i(vec: &Vec<bool>) -> u64 {
    let mut ret = 0;
    for i in 0..vec.len() {
        if vec[i] {
            ret |= 1 << i;
        }
    }
    ret
}

fn bit_criteria(vec: &Vec<Vec<bool>>, search: Common, default: bool) -> Vec<bool> {
    let mut values = vec.clone();
    for bit in (0..values[0].len()).rev() {
        let column: Vec<bool> = values.iter().map(|num| num[bit]).collect();
        let keep = match most_common(&column) {
            Common::One => (search == Common::One),
            Common::Zero => (search == Common::Zero),
            Common::Equal => default,
        };
        values.retain(|num| num[bit] == keep);
        if values.len() == 1 {
            break;
        }
    }
    values[0].clone()
}

#[aoc(day3, part2)]
pub fn part2(input: &Vec<Vec<bool>>) -> u64 {
    let ox = vb_i(&bit_criteria(&input, Common::One, true));
    let co2 = vb_i(&bit_criteria(&input, Common::Zero, false));

    ox * co2
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
