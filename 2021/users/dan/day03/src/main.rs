use std::fs;
use std::env;

// Counts the ones in every column of the provided lines slice
// Returns a vector of one counts (indexed by column, 0-based)
fn count_ones(lines: &[String]) -> Vec<u64>
{
    let mut one_count: Vec<u64> = vec![0; lines[0].len()];

    // Count the # of 1s in each column of the numbers
    for line in lines
    {
        line.chars().enumerate().for_each(|x| {
            if x.1 == '1'{
                one_count[x.0] += 1;
            }
        });
    }

    return one_count;
}

// Counts lines which have char (ch) at (idx)
fn count_idx(lines: &[String], idx: usize, ch: char) -> u64
{
    return lines.iter().fold(0, |sum,x| {
        sum + (x.chars().nth(idx).unwrap() == ch) as u64
    });
}

// Given the sorted input data, uses the "bit criteria" to identify the
// best match value in the input. Two flavors - "keep greater" and "keep lesser"
// "Keep greater" checks for the most common bit in each column
// "Keep lesser" checks for the lest common bit in each column
fn eval_bit_criteria(sorted_lines: &[String], keep_greater: bool) -> u64
{
    let mut curr_lines: Vec<String> = sorted_lines.to_vec();
    let char_len = sorted_lines[0].len();

    // Work through the bit criteria for each bit in the input
    // NOTE that we are narrowing our view of the data each cycle
    // This isn't just a binary search for the closest match to the
    // most/least common bit in each column
    for idx in 0..char_len
    {
        //for line in &curr_lines { println!("{}", line); }

        // Count freq of bits in the current selection
        let ones_count = count_idx(&curr_lines, idx, '1');

        let desired_char = match keep_greater {
            // Greater - '1' if count is at least half (inclusive)
            true => if ones_count*2 >= (curr_lines.len() as u64)
                { '1' } else { '0' },
            // Lesser - '1' if count is less than half (exclusive)
            false => if ones_count*2 < (curr_lines.len() as u64)
                { '1' } else { '0' }
        };

        //println!("[{}], Ones Count ({}), Desired '{}'", idx, ones_count, desired_char);

        // Locate start/stop of range we want to keep
        let start_idx = curr_lines.iter().enumerate()
            .filter(|line| line.1.chars().nth(idx).unwrap() == desired_char).next().unwrap().0;
        let stop_idx = curr_lines.iter().enumerate()
            .filter(|line| line.1.chars().nth(idx).unwrap() == desired_char).last().unwrap().0;

        let range_len = stop_idx - start_idx + 1;

        // Rotate our current lines to first occurence of desired bit
        curr_lines.rotate_left(start_idx);
        // Truncate current lines to index of last desired bit (discarding stuff we don't want)
        curr_lines.truncate(range_len);

        if curr_lines.len() == 1 { break; } // Stop when we have a single item left
    }

    return u64::from_str_radix(&curr_lines[0], 2).unwrap();
}

fn part_one(sorted_lines: &[String])
{
    let ones_count = count_ones(&sorted_lines);
    let half_lines = (sorted_lines.len() / 2) as u64;

    // Compute the gamma value by using the HIGHEST count in each column
    let gamma = ones_count.iter().fold(0, |mut sum, x| {
        sum <<= 1;
        if x > &half_lines { sum += 1; }
        return sum
    });

    // Compute the epsilon value by using the LOWEST count in each column
    let epsilon = ones_count.iter().fold(0, |mut sum, x| {
        sum <<= 1;
        if x < &half_lines { sum += 1; }
        return sum
    });

    println!("Part 1: gammma({}) * epsilon({}) = {}", gamma, epsilon, gamma*epsilon);
}

fn part_two(sorted_lines: &[String])
{
    // Evaluate the input based on "bit criteria"

    let oxy_rating = eval_bit_criteria(&sorted_lines, true);
    println!("Part 2: Oxygen Rating = {}", oxy_rating);

    let co2_rating = eval_bit_criteria(&sorted_lines, false);
    println!("Part 2: CO2 Rating = {}", co2_rating);
    println!("Part 2 Answer: {}", oxy_rating * co2_rating);
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");

    let mut sorted_lines: Vec<String> = input.lines()
        .map(|x| x.to_string() )
        .collect();
    sorted_lines.sort();
    //for line in sorted_lines.iter() { println!("{}", line); }

    part_one(&sorted_lines);

    part_two(&sorted_lines);

}
