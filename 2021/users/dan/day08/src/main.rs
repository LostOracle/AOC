use std::fs;
use std::env;
use std::collections::HashMap;

#[derive(Default)]
struct DigitNote {
    signals: Vec<String>,
    outputs: Vec<String>
}

fn calc_missing(signal: &String) -> String {
    let mut missing_seg = "abcdefg".to_string();
    missing_seg.retain(|c| !signal.contains(c) );
    //println!("{}", missing_seg);
    return missing_seg;
}

fn sorted(s: String) -> String {
    let mut chars = s.chars().collect::<Vec<_>>();
    chars.sort_by(|a,b| a.cmp(b));

    chars.iter().collect::<String>()
}

// Calculates a mapping between signals and digits on the 7-seg display
fn calc_digit_map(signals: &Vec<String>) -> HashMap<u64, String> {
    // Map all signals to digits
    let mut digit_map = HashMap::new();
    
    let mut signals = signals.clone().to_owned();
    //let mut signals = digit_notes.iter().next().unwrap().signals.clone().to_owned();

    // Some items have a unique segment count - 1, 4, 7, 8

    let one = signals.iter().position(|x| x.len() == 2).unwrap();
    digit_map.insert(1, signals[one].clone());
    signals.remove(one);

    let four = signals.iter().position(|x| x.len() == 4).unwrap();
    digit_map.insert(4, signals[four].clone());
    signals.remove(four);

    let seven = signals.iter().position(|x| x.len() == 3).unwrap();
    digit_map.insert(7, signals[seven].clone());
    signals.remove(seven);

    let eight = signals.iter().position(|x| x.len() == 7).unwrap();
    digit_map.insert(8, signals[eight].clone());
    signals.remove(eight);
    
    // 9 has 6/7 segments, and the missing segment is not in 4 or 1
    let nine = signals.iter().position(|x| {
        let len_ok = x.len() == 6;

        let missing_seg = calc_missing(x);
        let seg_in_four = digit_map.get(&4).unwrap().contains(&missing_seg);
        let seg_in_one = digit_map.get(&1).unwrap().contains(&missing_seg);
        len_ok && !seg_in_four && !seg_in_one
    }).unwrap();
    digit_map.insert(9, signals[nine].clone());
    signals.remove(nine);

    // 0 has 6/7 segments, and the missing segment is in 4 but not in 1
    let zero = signals.iter().position(|x| {
        let len_ok = x.len() == 6;

        let missing_seg = calc_missing(x);
        let seg_in_four = digit_map.get(&4).unwrap().contains(&missing_seg);
        let seg_in_one = digit_map.get(&1).unwrap().contains(&missing_seg);
        len_ok && seg_in_four && !seg_in_one
    }).unwrap();
    digit_map.insert(0, signals[zero].clone());
    signals.remove(zero);

    // 6 has 6/7 segments, and is the only remaining 6-segment after 0 and 9
    let six = signals.iter().position(|x| x.len() == 6).unwrap();
    digit_map.insert(6, signals[six].clone());
    signals.remove(six);

    // 3 has 5 segments, and contains all segments from 1
    let three = signals.iter().position(|x| {
        let len_ok = x.len() == 5;

        let shares_all = digit_map.get(&1).unwrap()
            .chars().all(|c| x.contains(c));

        len_ok && shares_all
    }).unwrap();
    digit_map.insert(3, signals[three].clone());
    signals.remove(three);

    // 2 has 5 segments, and contains the missing segment from 9
    let two = signals.iter().position(|x| {
        let len_ok = x.len() == 5;

        let missing_nine = calc_missing(digit_map.get(&9).unwrap());

        len_ok && x.contains(&missing_nine)
    }).unwrap();
    digit_map.insert(2, signals[two].clone());
    signals.remove(two);

    // 5 is the last digit we have
    let five = signals.iter().position(|_| true).unwrap();
    digit_map.insert(5, signals[five].clone());
    signals.remove(five);

    //println!("Signals clear? {}", signals.len());
    //println!("digits: {:?}", digit_map);
    return digit_map;
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");

    // The notations are as follows:
    // 10 space-separated strings (up to 7 chars) | 4 space-separated strings (up to 7 chars)
    // The space-separated strings indicate segments of a 7-seg display
    let digit_notes : Vec<DigitNote> = input.lines()
        .map(|line| {
            let tokens = line.split_whitespace();

            let mut dn = DigitNote::default();

            let mut parse_signals = true;
            for t in tokens {
                match parse_signals { 
                    true => {
                        if t.ne("|") { dn.signals.push(sorted(t.to_string())); }
                        else { parse_signals = false; } },
                    false => { dn.outputs.push(sorted(t.to_string())); }
                }
            }

            return dn;
        }).collect();

    //for d in digit_notes {
    //    println!("{}, {}",
    //        d.signals.join(" "),
    //        d.outputs.join(" "));
    //}

    // Part 1
    // Count # of strings in output matching specified lengths
    let segments = vec![ 2, 3, 4, 7 ];
    let digit_count = digit_notes.iter()
        .fold(0, |sum,row| {
            sum + row.outputs.iter()
                .filter(|x| segments.contains(&x.len()) ).count()
        });


    println!("Part 1: {}", digit_count);

    // Part 2 - decode all strings and sum
    let mut curr_num: u64 = 0;
    let mut sum: u64 = 0;


    for note in digit_notes.iter() {
        let digit_map = calc_digit_map(&note.signals);
        for out in note.outputs.iter() {
            //println!("out: {}", out);
            let digit = digit_map.iter()
                .find_map(|(k, v)| if v == out { Some(k) } else { None }).unwrap();

            curr_num *= 10;
            curr_num += digit;
        }
        println!("Decoded number: {}", curr_num);
        sum += curr_num;
        curr_num = 0;
    }

    println!("Sum: {}", sum);
}
