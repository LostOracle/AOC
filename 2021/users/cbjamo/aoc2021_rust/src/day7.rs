
#[aoc_generator(day7)]
pub fn input_gen(input: &str) -> (Vec<i32>, i32, i32) {
    let mut crabs = Vec::new();
    let mut max = 0;
    let mut min = i32::MAX;
    for chunk in input.split(",") {
        let crab = chunk.parse::<i32>().unwrap();
        if crab < min {
            min = crab;
        } else if crab > max {
            max = crab;
        }
        crabs.push(crab);
    }
    (crabs, min, max)
}

#[aoc(day7, part1)]
pub fn part1((crabs, min, max): &(Vec<i32>, i32, i32)) -> i32 {
    let mut min_cost = i32::MAX;
    for i in *min..*max {
        let cost: i32 = crabs.iter().map(|crab| (crab - i).abs()).sum();
        if cost < min_cost {
            min_cost = cost
        }
    }

    min_cost
}

#[aoc(day7, part2)]
pub fn part2((crabs, min, max): &(Vec<i32>, i32, i32)) -> i32 {
    let mut min_cost = i32::MAX;
    for i in *min..*max {
        let cost = crabs.iter().map(|crab| {
            let dist = (crab - i).abs();
            let mut cost = 0;
            for i in 0..dist {
                cost += i+1;
            }
            cost
        }).sum();
        if cost < min_cost {
            min_cost = cost
        }
    }

    min_cost
}

#[cfg(test)]
mod tests {
    use super::*;

    const INPUT: &str = "16,1,2,0,4,2,7,1,2,14";

    #[test]
    fn testpart1() {
        assert_eq!(part1(&input_gen(INPUT)), 37);

    }
    #[test]
    fn testpart2() {
        assert_eq!(part2(&input_gen(INPUT)), 168);

    }
}

