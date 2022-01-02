use std::fs;
use std::env;
use std::collections::HashMap;

fn inc(pos: usize, val: usize) -> usize {
    let new_pos = (pos + val) % 10;

    // Our numbers are biased - 0 means "10"
    if new_pos == 0 { 10 }
    else { new_pos }
}

struct Die {
    val: usize,
    num_rolls: usize,
}

impl Die {
    fn new() -> Die {
        Die{ val: 1, num_rolls: 0 }
    }

    fn roll(&mut self) -> usize {
        let old_val = self.val;

        let add_one = old_val == 100;
        self.val = ((old_val + 1) % 101)
            + match add_one { true => 1, false => 0 };

        self.num_rolls += 1;

        old_val
    }
}

#[derive(Copy,Clone)]
struct Player {
    id: usize,
    pos: usize,
    score: usize,
}

struct Roll {
    val: usize,
    freq: u64,
}

static FREQS: [Roll; 7] = [
        Roll{ val: 3, freq: 1},
        Roll{ val: 4, freq: 3},
        Roll{ val: 5, freq: 6},
        Roll{ val: 6, freq: 7},
        Roll{ val: 7, freq: 6},
        Roll{ val: 8, freq: 3},
        Roll{ val: 9, freq: 1},
    ];

type PWins = (u64,u64);

#[derive(Copy,Clone,PartialEq,Eq,Hash)]
struct GameState {
    p1_pos: usize,
    p2_pos: usize,
    p1_score: usize,
    p2_score: usize,
    p1_active: bool,
}

// Old, uncached version - takes around 10s
/*
fn play(players: &Vec<Player>, player_idx: usize, cache: &mut HashMap<GameState, PWins>) -> PWins {
    // Check if any player has won
    if players[0].score >= 21 {
        return (1,0);
    }

    if players[1].score >= 21 {
        return (0,1);
    }

    // Calculate new active player
    let next_player = if player_idx == 0 { 1 } else { 0 };

    // Now, we know that we must explore all possibilities. Therefore, 
    // we use a list of possibilities and frequencies to reduce the # of recursive calls
    let mut p0_win: u64 = 0;
    let mut p1_win: u64 = 0;
    FREQS.iter().for_each(|r| {
        // Update score and check if active player has won
        let mut next_players = players.clone();
        let p = &mut next_players[player_idx];
        p.pos = inc(p.pos, r.val);
        p.score += p.pos;

        let scores = play(&next_players, next_player, cache);
        p0_win += scores.0 * r.freq;
        p1_win += scores.1 * r.freq;
    });

    (p0_win, p1_win)
}*/

// Cached version - much faster
fn play_cache(state: GameState, cache: &mut HashMap<GameState, PWins>) -> PWins {
    // Check if any playe rhas won
    if state.p1_score >= 21 {
        return (1,0);
    }

    if state.p2_score >= 21 {
        return (0,1);
    }

    // Now, we know that we must explore all possibilities. Therefore, 
    // we use a list of possibilities and frequencies to reduce the # of recursive calls
    let mut p1_win: u64 = 0;
    let mut p2_win: u64 = 0;
    FREQS.iter().for_each(|r| {
        let mut state = state.clone();
        // Update score and check if active player has won
        if state.p1_active {
            state.p1_pos = inc(state.p1_pos, r.val);
            state.p1_score += state.p1_pos;
            state.p1_active = false;
        }
        else {
            state.p2_pos = inc(state.p2_pos, r.val);
            state.p2_score += state.p2_pos;
            state.p1_active = true;
        }

        let scores = match cache.get(&state) {
            Some(&score) => score,
            None => {
                let score = play_cache(state, cache);
                cache.insert(state, score);
                score
            }
        };

        p1_win += scores.0 * r.freq;
        p2_win += scores.1 * r.freq;
    });

    (p1_win, p2_win)
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");

    let mut id = 0;

    let players: Vec<Player> = input.lines()
        .map(|line| {
            let start_pos = line.split(':')
                .skip(1)
                .map(|x| { println!("{}", x); x.trim().parse::<usize>().unwrap() })
                .next().unwrap();
            
            id += 1;
            Player{ id: id, pos: start_pos, score: 0 }
        })
        .collect();

    let player_max = players.len();
    let mut p_idx = 0;
    let mut die = Die::new();

    let mut part1_players = players.clone();

    loop {
        // Roll 3 times
        let total_roll = die.roll() + die.roll() + die.roll();
        part1_players[p_idx].pos = inc(part1_players[p_idx].pos, total_roll);
        part1_players[p_idx].score += part1_players[p_idx].pos;

        p_idx = (p_idx + 1) % player_max;
        if part1_players.iter().any(|p| p.score >= 1000) { break; }
    }

    println!("Results after {} rolls: {}", die.num_rolls,
        part1_players.iter().fold(String::new(), |sum, p|
            sum + &format!("\nP{}: pos={}, score={}", p.id, p.pos, p.score)));

    let min_score = part1_players.iter().min_by(|x, y| x.score.cmp(&y.score)).unwrap().score;

    println!("Part1: Losing score * die rolls = {}", min_score * die.num_rolls);

    let mut cache = HashMap::<GameState, PWins>::new();
    let state = GameState {
        p1_pos: players[0].pos,
        p2_pos: players[1].pos,
        p1_score: 0,
        p2_score: 0,
        p1_active: true,
    };
    let part2_wins = play_cache(state, &mut cache);

    println!("Part2: P1 Freq = {}, P2 Freq = {}",
        part2_wins.0,
        part2_wins.1);
}
