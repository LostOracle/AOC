use std::str::FromStr;

pub enum Direction {
    Forward,
    Down,
    Up,
}
impl FromStr for Direction {
    type Err = String;
    fn from_str( string: &str ) -> Result<Self, Self::Err> {
        match string
        {
            "forward" => Ok(Direction::Forward),
            "down" => Ok(Direction::Down),
            "up" => Ok(Direction::Up),
            _ => Err(format!("Failed to parse [{}]", string))
        }
    }
}

type Command = (Direction, i32);

#[aoc_generator(day2)]
pub fn input_gen(input: &str) -> Vec<Command> {
    input.lines().map(|l| {
                      let s = l.split(' ').collect::<Vec<&str>>();
                      let dir = s[0].parse::<Direction>().unwrap();
                      let a = s[1].parse::<i32>().unwrap();
                      (dir, a)
    }).collect()
}

#[aoc(day2, part1)]
pub fn part1(input: &Vec<Command>) -> i32 {
    let mut x: i32 = 0;
    let mut y: i32 = 0;
    for c in input {
        match c.0 {
            Direction::Forward => x += c.1,
            Direction::Up => y -= c.1,
            Direction::Down => y += c.1,
        }
    }

    x * y
}

#[aoc(day2, part2)]
pub fn part2(input: &Vec<Command>) -> i32 {
    let mut x: i32 = 0;
    let mut y: i32 = 0;
    let mut aim: i32 = 0;
    for c in input {
        match c.0 {
            Direction::Forward => {x += c.1; y += aim * c.1},
            Direction::Up => aim -= c.1,
            Direction::Down => aim += c.1,
        }
    }

    x * y
}

