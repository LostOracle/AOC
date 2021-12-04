/*#[aoc_generator(day3)]
pub fn input_generator(input: &str) -> Vec<(Heading, i32)>
{
  input.lines().map(|line|
    {
      let mut split_res = line.split_whitespace();
      let heading: Heading = split_res.next().unwrap().parse::<Heading>().unwrap();
      let val: i32 = split_res.next().unwrap().parse::<i32>().unwrap();
      (heading, val)
    }).collect()
}*/

#[aoc(day3, part1)]
pub fn solve_part1(input: &str) -> isize
{
  let mut counts = Vec::<u32>::new();
  let mut total = 0;

  for line in input.lines()
  {
    if counts.len() == 0
    {
      counts.resize(line.len(), 0);
    }
    for (bit, count) in line.chars().zip(counts.iter_mut())
    {
      if bit == '1'
      {
        *count += 1;
      }
    }
    total += 1;
  }

  //println!("{:?}", counts);

  let gamma_bits: String = counts.into_iter().map(|count|
    if count > total/2
    {
      '1'
    }
    else
    {
      '0'
    }
  ).collect();

  let gamma = isize::from_str_radix(gamma_bits.as_str(), 2).unwrap();

  let mut mask_bits: Vec<char> = Vec::<_>::new();
  mask_bits.resize(gamma_bits.len(), '1');

  let mask_str: String = mask_bits.into_iter().collect();

  let mask_val = isize::from_str_radix(mask_str.as_str(), 2).unwrap();
  let epsilon = !gamma & mask_val;
  //println!("gamma_bits {} total {}", gamma_bits, total);
  return gamma*epsilon;
}

#[cfg(test)]
mod tests
{
  use super::*;

  #[test]
  fn testinput()
  {
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

assert_eq!(solve_part1(input), 198)
  }
}
