use std::fmt;
use std::str::FromStr;

const MAX_AGE: usize = 9;

#[derive(Clone, Debug, PartialEq)]
pub struct School {
    counts: [u64; MAX_AGE],
    days: u32,
}
impl School {
    fn day(&mut self) {
        let mut new_counts = [0; MAX_AGE];
        for i in (0..MAX_AGE).rev() {
            if i == 0 {
                new_counts[6] += self.counts[i];
                new_counts[8] += self.counts[i];
            } else {
                new_counts[i-1] += self.counts[i];
            }
        }
        self.counts = new_counts;
        self.days += 1;
    }
    fn count(&self) -> u64 {
        let mut count = 0;
        for i in 0..MAX_AGE {
            count += self.counts[i];
        }
        count
    }
}

impl fmt::Display for School {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "Day:{:2}, {:?}, {}", self.days, self.counts, self.count())
    }
}

#[aoc_generator(day6)]
pub fn input_gen(input: &str) -> School {
    let school: Vec<usize> = input.split(",").filter_map(|element| {
        match element.parse::<>() {
            Ok(x) => Ok(x),
            Err(e) => Err(e)
        }.ok()
    }).collect();
    let mut counts = [0; MAX_AGE];
    for fish in school {
        counts[fish] += 1;
    }
    School{ counts:counts, days:0 }
}

#[aoc(day6, part1)]
pub fn part1(input: &School) -> u64 {
    let mut school = input.clone();
    
    for _i in 0..80 {
        school.day();
    }

    school.count()
}

#[aoc(day6, part2)]
pub fn part2(input: &School) -> u64 {
    let mut school = input.clone();
    
    for _i in 0..256 {
        school.day();
    }

    school.count()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn testpart1() {
        let input = "3,4,3,1,2";

        assert_eq!(part1(&input_gen(input)), 5934);

    }
    #[test]
    fn testpart2() {
        let input = "3,4,3,1,2";

        assert_eq!(part2(&input_gen(input)), 26984457539);

    }
}
