
#[aoc_generator(day1)]
pub fn input_generator(input: &str) -> Vec<u32> {
  input.lines().map(|x| {
    x.parse::<u32>().unwrap()
  }).collect()
}

#[aoc(day1, part1)]
pub fn solve_part1(input: &[u32]) -> u32 {
  let mut prev:u32 = input[0];
  let mut answer = 0;
  for x in input {
    if *x > prev {
      answer = answer + 1;
    }
    prev = *x;
  }
  return answer;
}

#[aoc(day1, part2)]
pub fn solve_part2(input: &[u32]) -> u32 {
  let mut prev:u32  = input[0] + input[1] + input[2];
  let mut answer = 0;
  for x in input.windows(3) {
    let sum = x.iter().sum();
    if sum > prev {
      answer = answer + 1;
    }
    prev = sum;
  }
  return answer;
}
