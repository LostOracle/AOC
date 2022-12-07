use std::fs;

fn main() {
    let input = fs::read_to_string("input.txt")
        .expect("Unable to open input.txt");

    // Given the whole input file,
    // Separate into groups whenever we find two adjacent newlines (i.e. an empty line)
    // Each group is a collection of lines, and each line is a u64
    // We want the whole group as a vector
    // And the collection of groups as a vector
    let elf_packs: Vec<Vec<u64>> = input
        .split("\n\n")
        .map(|group| {
            group
                .lines()
                .map(|entry| entry.parse().expect("not a number"))
                .collect()
        })
        .collect();

    //println!("{:?}", elf_packs);

    // Now we want to find the highest sum among the packs
    // So we will iterate through the packs, keeping the highest sum
    let max_pack = elf_packs
        .iter()
        .fold(0, |accum, pack| {
            let pack_mass = pack.iter().fold(0, |accum, elem| accum + elem );
            if accum > pack_mass { accum }
            else { pack_mass }
        });

    println!("Part 1: Max pack value: {}", max_pack);

    // For part 2, sort the packs by size, and then get the top 3 and sum them
    let mut sorted_packs: Vec<u64> = elf_packs
        .iter()
        .map(|pack| pack.iter().sum())
        .collect();

    sorted_packs.sort();

    let top_three: u64 = sorted_packs
        .iter()
        .rev()
        .take(3)
        .sum();

    println!("Part 2: Top three packs: {}", top_three);
}
