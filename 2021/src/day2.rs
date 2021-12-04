use std::str::FromStr;

pub enum Heading
{
  Forward,
  Down,
  Up
}

impl FromStr for Heading
{
  type Err = String;

  fn from_str(s: &str) -> Result<Self, Self::Err>
  {
    match s
    {
        "forward" => Ok(Heading::Forward),
        "down" => Ok(Heading::Down),
        "up" => Ok(Heading::Up),
        _ =>  Err(format!("Invalid heading: {}", s))
    }
  }
}
#[aoc_generator(day2)]
pub fn input_generator(input: &str) -> Vec<(Heading, i32)>
{
  input.lines().map(|line|
    {
      let mut split_res = line.split_whitespace();
      let heading: Heading = split_res.next().unwrap().parse::<Heading>().unwrap();
      let val: i32 = split_res.next().unwrap().parse::<i32>().unwrap();
      (heading, val)
    }).collect()
}

#[aoc(day2, part1)]
pub fn solve_part1(input: &[(Heading, i32)]) -> i32 
{
  let mut depth: i32 = 0;
  let mut dist: i32 = 0;
  for (heading, val) in input
  {
    match heading
    {
      Heading::Forward => dist += val,
      Heading::Up => depth -= val,
      Heading::Down => depth += val,
    }
  }
  return depth*dist;
}

#[aoc(day2, part2)]
pub fn solve_part2(input: &[(Heading, i32)]) -> i32 
{
  let mut depth: i32 = 0;
  let mut dist: i32 = 0;
  let mut aim: i32 = 0;
  for (heading, val) in input
  {
    match heading
    {
      Heading::Forward =>
      {
        dist += val;
        depth += val*aim;
      },
      Heading::Up => aim -= val,
      Heading::Down => aim += val,
    }
  }
  return depth*dist;
}
