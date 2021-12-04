
#[aoc_generator(day4)]
pub fn input_gen(input: &str) -> Vec<u64> {
    input.lines().map(|l| {
        u64::from_str_radix(l, 2).unwrap()
    }).collect()
}

#[aoc(day4, part1)]
pub fn part1(input: &Vec<u64>) -> u64 {
    input.iter().sum()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn testinput() {
        let input = "0010001010";

        assert_eq!(part1(&input_gen(input)), 198);

    }
}
