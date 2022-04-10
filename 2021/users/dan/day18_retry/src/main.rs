use std::fs;
use std::env;

extern crate regex;
use regex::{Captures, Regex};

// Adds two snail #s together (concatenates)
fn snail_add(left: &str, right: &str) -> String {
    if left.is_empty() {
        return right.to_string()
    }

    if right.is_empty() {
        return left.to_string()
    }

    "[".to_string() + left + "," + right + "]"
}

// Splits the first item > 9 in the string into a full node
fn snail_split(in_str: &str, got_one: &mut bool) -> String {
    let mut out = String::new();
    *got_one = false;

    // We use split_inclusive because it lets us keep the delimiters
    // For each tokenized slice, we either copy it straight to output, or split
    // After a single split, we revert to copying
    for tok in in_str.split_inclusive(|ch| ['[', ']', ','].iter().any(|x| x.eq(&ch))) {
        if !*got_one {
            match tok {
                "[" | "]" | "," => { out.push_str(tok); },
                _ => {
                    // Convert the token to a #
                    let last_tok = tok.len() - 1;
                    let num = tok[..last_tok].parse::<u32>().unwrap();
                    if num > 9 {
                        // Split big numbers into two smaller #s
                        out += format!("[{},{}]",
                            (num / 2),
                            (num as f32 / 2 as f32).ceil() as u32).as_str();
                        out += &tok[last_tok..]; // Add on the ending token

                        *got_one = true; // Don't check for any more splits
                    }
                    else
                    {
                        out += tok;
                    }
                }
            }
        }
        else
        {
            out += tok;
        }
    }

    out
}

fn get_num(tok: &str) -> u32 {
    //println!("TOK: {}", tok);
    tok[..tok.len()-1].parse::<u32>().unwrap()
}

fn do_explode<'a>(toks: &mut impl Iterator<Item = &'a str>, val: u32) -> Vec<String> {
    let mut out_toks = Vec::<String>::new();
    let mut found = false;
    while let Some(tok) = toks.next() {
        match tok {
            "[" | "]" | "," => { out_toks.push(tok.to_string()); },
            _ => {
                if !found {
                    // Convert the token to a #
                    let num = get_num(tok);

                    // If we find a number, add left side number
                    let num = num + val;
                    let num_str = num.to_string() + &tok[tok.len()-1..];
                    out_toks.push(num_str);

                    found = true; // We only want to add once
                }
                else {
                    out_toks.push(tok.to_string());
                }
            }
        }
    }

    out_toks
}

fn snail_explode(in_str: &str, got_one: &mut bool) -> String {
    *got_one = false;

    // Iterate through string, copying as we go, until we hit depth > 4 of '['
    // At that point, capture the bracketed pair [aa,bb]
    // aa goes left (reverse iter on output string).
    // bb goes right (memory for remaining string copy)
    
    let mut out = String::new();
    let mut depth = 0;
    let mut in_idx = 0;

    loop {
        if in_idx >= in_str.len() { break; }

        // Process next char
        match &in_str[in_idx..in_idx+1] {
            "[" => { depth += 1; },
            "]" => { depth -= 1; },
            _ => {}
        }

        // If this is too deep, stop immediately to explode
        if depth > 4 { break; }

        // Otherwise, copy the character to output
        out += &in_str[in_idx..in_idx+1];
        in_idx += 1;
    }

    // If we're at the end of the string, return it
    if in_idx >= in_str.len() { return out; }

    // Otherwise, we must explode
    *got_one = true;

    // Get the exploding numbers
    let mut right_toks = in_str[in_idx+1..].split_inclusive(|ch| ['[', ']', ','].iter().any(|x| x.eq(&ch)));
    let aa = get_num(right_toks.next().unwrap());
    let bb = get_num(right_toks.next().unwrap());

    // First, we get the left-going number
    // Then we find the closest number (going left) to add to
    // If one doesn't exist, the left-going number "falls off"

    let mut left_toks = in_str[..in_idx].split_inclusive(|ch| ['[', ']', ','].iter().any(|x| x.eq(&ch))).rev();

    let out_toks = do_explode(&mut left_toks, aa);

    // Now re-generate output using the output tokens
    out = out_toks.iter().rev().fold(String::new(), |acc, x| acc + x);

    // Add a 0 at current position in output
    out += "0";

    // Next, we get the right-going number
    // Then we find the closest number (going right) to add to
    let out_toks = do_explode(&mut right_toks, bb);
    out = out_toks.iter().fold(out, |acc, x| acc + x);

    out
}

fn snail_reduce(in_str: &str) -> String {
    let mut out: String = in_str.to_string();

    loop {
        let mut got_one = false;
        out = snail_explode(&out, &mut got_one);
        if !got_one {
            out = snail_split(&out, &mut got_one);
        }

        if !got_one { break; }
    }

    out
}

fn snail_sum(list: &Vec<String>) -> String {
    let mut result = String::new();
    for n in list {
        result = snail_add(&result, n);
        result = snail_reduce(&result); 
    }

    result
}

fn snail_magnitude(sum: &str) -> u32 {
    let pair_regex = Regex::new(r"\[(\d+),(\d+)\]").unwrap();
    let mut mod_sum = sum.to_string();

    let mut max_loop = 0;
    loop {
        // Check for matches. If no matches, bail
        if !pair_regex.is_match(&mod_sum) { break; }
        if max_loop > 10 { break; }
        max_loop += 1;

        mod_sum = pair_regex.replace_all(&mod_sum, |caps: &Captures| {
            let left = &caps[1].parse::<u32>().unwrap();
            let right = &caps[2].parse::<u32>().unwrap();
            (left * 3 + right * 2).to_string()
        });
    }

    mod_sum.parse::<u32>().unwrap()
}

fn part_2(numbers: &Vec<String>) -> u32 {
    let mut curr_idx = 0;
    let mut max_mag = 0;

    loop {
        if curr_idx >= numbers.len() { break; }

        for i in curr_idx+1..numbers.len() {
            let left = &numbers[curr_idx];
            let right = &numbers[i];

            let sum = snail_add(&left, &right);
            let sum = snail_reduce(&sum);
            let mag_a = snail_magnitude(&sum);

            let sum = snail_add(&right, &left);
            let sum = snail_reduce(&sum);
            let mag_b = snail_magnitude(&sum);

            if mag_a > max_mag { max_mag = mag_a; }
            if mag_b > max_mag { max_mag = mag_b; }
        }

        curr_idx += 1;
    }

    max_mag
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");

    let numbers: Vec<String> = input.lines()
        .map(|x| x.to_string())
        .collect();

    let sum = snail_sum(&numbers);
    let mag = snail_magnitude(&sum);
    println!("Part 1: {}", sum);
    println!("Part 1: {}", mag);

    // Part 2 - check all possible combos (and orderings) of 2 numbers in this list
    part_2(&numbers);

    println!("Max mag: {}", max_mag);
}

mod tests{
    use super::*;

    #[test]
    fn test_add1() {
        let result = snail_add("[1,2]", "[[3,4],5]");
        assert_eq!("[[1,2],[[3,4],5]]", result);
    }

    #[test]
    fn test_split1() {
        let mut got_one: bool = false;
        let result = snail_split("[10]", &mut got_one);
        assert_eq!("[[5,5]]", result);
        assert_eq!(true, got_one);
    }

    #[test]
    fn test_split2() {
        let mut got_one = false;
        let result = snail_split("[11]", &mut got_one);
        assert_eq!("[[5,6]]", result);
        assert_eq!(true, got_one);
    }

    #[test]
    fn test_split3() {
        let mut got_one = false;
        let result = snail_split("[[[[0,7],4],[15,[0,13]]],[1,1]]", &mut got_one);
        assert_eq!("[[[[0,7],4],[[7,8],[0,13]]],[1,1]]", result);
        assert_eq!(true, got_one);
    }

    #[test]
    fn test_split4() {
        let mut got_one = false;
        let result = snail_split("[[[[0,7],4],[[7,8],[0,13]]],[1,1]]", &mut got_one);
        assert_eq!("[[[[0,7],4],[[7,8],[0,[6,7]]]],[1,1]]", result);
        assert_eq!(true, got_one);
    }

    #[test]
    fn test_explode1() {
        let mut got_one = false;
        let result = snail_explode("[10]", &mut got_one);
        assert_eq!("[10]", result); // no explode on this one
        assert_eq!(false, got_one);
    }

    #[test]
    fn test_explode2() {
        let mut got_one = false;
        let result = snail_explode("[[[[[9,8],1],2],3],4]", &mut got_one);
        assert_eq!("[[[[0,9],2],3],4]", result);
        assert_eq!(true, got_one);
    }

    #[test]
    fn test_explode3() {
        let mut got_one = false;
        let result = snail_explode("[7,[6,[5,[4,[3,2]]]]]", &mut got_one);
        assert_eq!("[7,[6,[5,[7,0]]]]", result);
        assert_eq!(true, got_one);
    }

    #[test]
    fn test_explode4() {
        let mut got_one = false;
        let result = snail_explode("[[6,[5,[4,[3,2]]]],1]", &mut got_one);
        assert_eq!("[[6,[5,[7,0]]],3]", result);
        assert_eq!(true, got_one);
    }

    #[test]
    fn test_explode5() {
        let mut got_one = false;
        let result = snail_explode("[[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]", &mut got_one);
        assert_eq!("[[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]", result);
        assert_eq!(true, got_one);
    }

    #[test]
    fn test_explode6() {
        let mut got_one = false;
        let result = snail_explode("[[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]", &mut got_one);
        assert_eq!("[[3,[2,[8,0]]],[9,[5,[7,0]]]]", result);
        assert_eq!(true, got_one);
    }

    #[test]
    fn test_sum1() {
        let input = vec![
            String::from("[1,1]"),
            String::from("[2,2]"),
            String::from("[3,3]"),
            String::from("[4,4]")
        ];

        let result = snail_sum(&input);
        assert_eq!("[[[[1,1],[2,2]],[3,3]],[4,4]]", result);
    }

    #[test]
    fn test_sum2() {
        let input = vec![
            String::from("[1,1]"),
            String::from("[2,2]"),
            String::from("[3,3]"),
            String::from("[4,4]"),
            String::from("[5,5]"),
        ];

        let result = snail_sum(&input);
        assert_eq!("[[[[3,0],[5,3]],[4,4]],[5,5]]", result);
    }

    #[test]
    fn test_sum3() {
        let input = vec![
            String::from("[1,1]"),
            String::from("[2,2]"),
            String::from("[3,3]"),
            String::from("[4,4]"),
            String::from("[5,5]"),
            String::from("[6,6]"),
        ];

        let result = snail_sum(&input);
        assert_eq!("[[[[5,0],[7,4]],[5,5]],[6,6]]", result);
    }

    #[test]
    fn test_sum4() {
        let input = vec![
            String::from("[[[0,[4,5]],[0,0]],[[[4,5],[2,6]],[9,5]]]"),
            String::from("[7,[[[3,7],[4,3]],[[6,3],[8,8]]]]"),
            String::from("[[2,[[0,8],[3,4]]],[[[6,7],1],[7,[1,6]]]]"),
            String::from("[[[[2,4],7],[6,[0,5]]],[[[6,8],[2,8]],[[2,1],[4,5]]]]"),
            String::from("[7,[5,[[3,8],[1,4]]]]"),
            String::from("[[2,[2,2]],[8,[8,1]]]"),
            String::from("[2,9]"),
            String::from("[1,[[[9,3],9],[[9,0],[0,7]]]]"),
            String::from("[[[5,[7,4]],7],1]"),
            String::from("[[[[4,2],2],6],[8,7]]"),
        ];

        let result = snail_sum(&input);
        assert_eq!("[[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]", result);
    }

    #[test]
    fn test_magnitude1() {
        assert_eq!(143, snail_magnitude("[[1,2],[[3,4],5]]"));
    }
    #[test]
    fn test_magnitude2() {
        assert_eq!(1384, snail_magnitude("[[[[0,7],4],[[7,8],[6,0]]],[8,1]]"));
    }
    #[test]
    fn test_magnitude3() {
        assert_eq!(445, snail_magnitude("[[[[1,1],[2,2]],[3,3]],[4,4]]"));
    }
    #[test]
    fn test_magnitude4() {
        assert_eq!(791, snail_magnitude("[[[[3,0],[5,3]],[4,4]],[5,5]]"));
    }
    #[test]
    fn test_magnitude5() {
        assert_eq!(1137, snail_magnitude("[[[[5,0],[7,4]],[5,5]],[6,6]]"));
    }
    #[test]
    fn test_magnitude6() {
        assert_eq!(3488, snail_magnitude("[[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]"));
    }

    #[test]
    fn test_max_mag() {
        let numbers = vec![
            String::from("[[[0,[5,8]],[[1,7],[9,6]]],[[4,[1,2]],[[1,4],2]]]"),
            String::from("[[[5,[2,8]],4],[5,[[9,9],0]]]"),
            String::from("[6,[[[6,2],[5,6]],[[7,6],[4,7]]]]"),
            String::from("[[[6,[0,7]],[0,9]],[4,[9,[9,0]]]]"),
            String::from("[[[7,[6,4]],[3,[1,3]]],[[[5,5],1],9]]"),
            String::from("[[6,[[7,3],[3,2]]],[[[3,8],[5,7]],4]]"),
            String::from("[[[[5,4],[7,7]],8],[[8,3],8]]"),
            String::from("[[9,3],[[9,9],[6,[4,9]]]]"),
            String::from("[[2,[[7,7],7]],[[5,8],[[9,3],[0,2]]]]"),
            String::from("[[[[5,2],5],[8,[3,7]]],[[5,[7,5]],[4,4]]]"),
        ];
        let max_mag = part_2(&numbers);

        assert_eq!(3993, max_mag);
    }
}
