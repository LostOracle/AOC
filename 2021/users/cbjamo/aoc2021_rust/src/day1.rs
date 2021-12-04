#[aoc_generator(day1)]
pub fn input_gen(input: &str) -> Vec<u32> {
    input.lines().map(|l| l.parse::<u32>().unwrap()).collect()
}

#[aoc(day1, part1, Loops)]
pub fn part1(input: &Vec<u32>) -> u32 {
    let mut last = 0;
    let mut ret = 0;
    for i in input {
        if *i > last {
            ret = ret + 1;
        }
        last = *i;
    }
    
    ret - 1
}

#[aoc(day1, part1, Iters)]
pub fn part1_iter(input: &Vec<u32>) -> u32 {
    input.windows(2).map(|x| if x[1] > x[0] { 1 } else { 0 }).sum()
}

#[aoc(day1, part2, Iters)]
pub fn part2_iter(input: &Vec<u32>) -> u32 {
    let window_sums: Vec<u32> = input.windows(3).map(|x| x[0]+x[1]+x[2]).collect();

    window_sums.windows(2).map(|x| if x[1] > x[0] { 1 } else { 0 }).sum()
}
