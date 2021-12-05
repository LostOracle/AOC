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

#[aoc(day3, part1, try1)]
pub fn solve_part1_1(input: &str) -> isize
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

#[aoc(day3, part1, try2)]
pub fn solve_part1_2(input: &str) -> isize
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

  let epsilon_bits: String = gamma_bits.chars().map(|bit_char|
  {
    if bit_char == '1'
    {
      '0'
    }
    else
    {
      '1'
    }
  }).collect();

  let gamma = isize::from_str_radix(gamma_bits.as_str(), 2).unwrap();
  let epsilon = isize::from_str_radix(epsilon_bits.as_str(), 2).unwrap();

  return gamma*epsilon;
}

fn get_most_common(nums: &Vec<String>, pos: usize) -> char
{
  let mut count = 0;
  let mut total = 0;
  for num in nums
  {
    if num.chars().nth(pos).unwrap() == '1'
    {
      count += 1;
    }
    total += 1;
  }
  //println!("count: {}, total {}, pos {}, total/2 {}", count, total, pos, total/2);
  if count >= (total - count)
  {
    return '1';
  }
  else
  {
    return '0';
  }
}

#[aoc(day3, part2)]
pub fn solve_part2(input: &str) -> isize
{
  let str_vec: Vec<String> = input.lines()
    .map(|l| l.to_string())
    .collect();

  let mut o2_vec = str_vec.clone();
  let width = o2_vec[0].len();

  for i in 0..width
  {
    let most_common = get_most_common(&o2_vec, i);
    //println!("most common: {}", most_common);
    o2_vec = o2_vec.into_iter().filter(|num|
      num.chars().nth(i).unwrap() == most_common
    ).collect();
    if o2_vec.len() == 1
    {
      break;
    }
    //println!("{:?}", o2_vec);
  }

  let o2 = &o2_vec[0];
  //println!("{:?}", o2);

  let mut co2_vec = str_vec.clone();
  for i in 0..width
  {
    let most_common = get_most_common(&co2_vec, i);
    //println!("most common: {}", most_common);
    co2_vec = co2_vec.into_iter().filter(|num|
      num.chars().nth(i).unwrap() != most_common
    ).collect();
    if co2_vec.len() == 1
    {
      break;
    }
    //println!("{:?}", co2_vec);
  }
  let co2 = &co2_vec[0];
  //println!("{:?}", co2);

  let o2 = isize::from_str_radix(o2.as_str(), 2).unwrap();
  let co2 = isize::from_str_radix(co2.as_str(), 2).unwrap();

  return o2*co2;
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

//assert_eq!(solve_part1_1(input), 198);
//assert_eq!(solve_part1_2(input), 198);
assert_eq!(solve_part2(input), 230);
  }
}
