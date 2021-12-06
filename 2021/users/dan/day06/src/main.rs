use std::fmt;
use std::fs;
use std::env;

struct Fish {
    id: u64,
    timer: u64, // Time until reproduction (counts down to 0)
    count: u64 // Number of fish at this stage
}

// Factory for creating fish, either with pre-existing timer, or birthing a fresh fish
#[derive(Default)]
struct FishFactory {
    curr_id: u64
}
impl FishFactory {
    fn new() -> FishFactory {
        Default::default()
    }
    fn fish(&mut self, c: u64) -> Fish {
        self.curr_id += 1;
        Fish{ id: self.curr_id, timer: 8, count: c }
    }
    fn fish_with_timer(&mut self, t: u64, c: u64) -> Fish {
        self.curr_id += 1;
        Fish{ id: self.curr_id, timer: t, count: c }
    }
}

impl fmt::Display for Fish {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "[{}]: timer({})", self.id, self.timer)
    }
}

fn run(pop: &mut Vec<Fish>, factory: &mut FishFactory) {
    // For each entry in the list, either decrease its timer
    // or spawn a new fish
    let mut spawn_count = 0;
    pop.iter_mut()
        .for_each(|x| { match x.timer {
            0 => {
                x.timer = 100; // Bogus # so we can delete it later
                spawn_count += x.count;
            },
            _ => { x.timer -= 1; }
        }});

    // Move current spawners into a "6" bucket
    pop.retain(|x| x.timer != 100);
    pop.push(factory.fish_with_timer(6, spawn_count));

    // Add new spawns to the end
    pop.push(factory.fish(spawn_count));
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");

    let mut lines = input.lines();

    let mut factory = FishFactory::new();
    let mut pop: Vec<Fish> = lines.next().unwrap()
        .split(',')
        .map(|x| factory.fish_with_timer(x.parse::<u64>().unwrap(), 1)).collect();

//    for f in &pop {
//        println!("{}", f);
//    }

    // Run population for N cycles
    for _i in 0..80 { run(&mut pop, &mut factory); }
    println!("Part 1: Pop after 80: {}", pop.iter().fold(0, |sum,x| sum + x.count));

    // Run population for an additional 176 cycles
    for _i in 0..176 { run(&mut pop, &mut factory); }
    println!("Part 3: Pop after 256: {}", pop.iter().fold(0, |sum,x| sum + x.count));
}
