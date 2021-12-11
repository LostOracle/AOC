use std::fs;
use std::env;


fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");


    let nav_syntax: Vec<String> = input.lines().map(|x| x.to_string()).collect();

    // Score trackers (part 1 and 2, respectively)
    let mut score = 0;
    let mut auto_scores = Vec::<u64>::new();

    // Use a stack to capture matching chunks
    let mut stack = Vec::<char>::new();
    for line in nav_syntax.iter() {
        for c in line.chars() {
            match c {
                '[' | '(' | '{' | '<' => { stack.push(c); }, // Start a chunk
                // The rest are closing a chunk (or a corruption)
                ']' => { if stack.pop() != Some('[') {
                    score += 57;
                    stack.clear();
                    break; } },
                ')' => { if stack.pop() != Some('(') {
                    score += 3;
                    stack.clear();
                    break; } },
                '}' => { if stack.pop() != Some('{') {
                    score += 1197;
                    stack.clear();
                    break; } },
                '>' => { if stack.pop() != Some('<') {
                    score += 25137;
                    stack.clear();
                    break; } },
                _ => { println!("Invalid character! {}", c); }
            }
        }

        // Part 2 - Fix incomplete lines
        // Since we clear the stack for each corruption,
        // the remaining items are unmatched parens
        if stack.len() > 0 {
            stack.reverse(); // We want to find missing parens from inner to outer
            let score = stack.iter().fold(0, |acc,x| {
                (acc * 5) + match x {
                    '(' => 1,
                    '[' => 2,
                    '{' => 3,
                    '<' => 4,
                    _ => 0 }
            });
            auto_scores.push(score);
        }

        stack.clear();
    }

    println!("Part 1: Score = {}", score);

    auto_scores.sort();
    println!("Part 2: Score = {}", auto_scores[auto_scores.len() / 2]);
}
