use std::fs;
use std::convert::TryFrom;

fn main() {
    let input = fs::read_to_string("input.txt")
        .expect("Unable to open input.txt");

    // Convert contents into a vector of ints
    let depths: Vec<_> = input.split_whitespace().map(|entry| {
        entry.parse::<u64>()
            .expect("Not a number!")
    }).collect();

    // Now scan the list for adjacent increases in depth
    let increasing_depths = depths
        .windows(2)
        .flat_map(<&[u64; 2]>::try_from)
        .filter(|w| {
        w[1] > w[0]
    }).count();

    println!("Part 1: {}", increasing_depths);

    let depth_threes: Vec<_> = depths.windows(3).map(|w| {
        w.iter().sum::<u64>()
    }).collect();
    let increasing_windows = depth_threes.windows(2).filter(|w| {
        w[1] > w[0]
    }).count();

    println!("Part 2: {}", increasing_windows);
}
