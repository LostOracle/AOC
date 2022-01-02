use std::fs;
use std::env;

struct Probe {
    x: isize,
    y: isize,
    x_vel: isize,
    y_vel: isize
}

struct Target {
    x_min: isize,
    x_max: isize,
    y_min: isize,
    y_max: isize,
}

fn step(p: &mut Probe) {
    p.x += p.x_vel;
    p.y += p.y_vel;
    p.x_vel =
        if p.x_vel > 0 { p.x_vel - 1 }
        else if p.x_vel < 0 { p.x_vel + 1 }
        else { 0 };
    
    p.y_vel -= 1;
}

fn in_target(t: &Target, p: &Probe) -> bool {
    p.x >= t.x_min
        && p.x <= t.x_max
        && p.y >= t.y_min
        && p.y <= t.y_max
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");

    // Parse target area out of the input
    // Run a simulation for every start vel in a range
    // Find the maximum height reached for the range
    let line = input.lines().next().unwrap();
    let x_start = line.find("=").unwrap()+1;
    let x_middle = line.find("..").unwrap();
    let x_end = line.find(",").unwrap();
    //println!("{}  {}", &line[x_start..x_middle], &line[x_middle..x_end]);
    let x_first = &line[x_start..x_middle].parse::<isize>().unwrap();
    let x_second = &line[x_middle+2..x_end].parse::<isize>().unwrap();

    let y_start = line.rfind("=").unwrap()+1;
    let y_middle = line.rfind("..").unwrap();
    let y_end = line.len();
    //println!("{}  {}", &line[y_start..y_middle], &line[y_middle..y_end]);
    let y_first = &line[y_start..y_middle].parse::<isize>().unwrap();
    let y_second = &line[y_middle+2..y_end].parse::<isize>().unwrap();

    let target = Target{ x_min: *x_first, x_max: *x_second, y_min: *y_first, y_max: *y_second };
    println!("Target area: x={}..{}, y={}..{}", target.x_min, target.x_max, target.y_min, target.y_max);

    let mut overall_max = 0;
    let mut target_count = 0;

    // Run the simulation
    // We know that x_vel can't be more than the max x value of the target area
    // Otherwise, we make some arbitrary guesses at y velocity range (yes, brute force)
    for x_vel in 0..target.x_max+1 {
        for y_vel in -1000..1000 {
            let mut step_num = 0;
            let mut probe = Probe{ x: 0, y: 0, x_vel: x_vel, y_vel: y_vel };
            let mut max_y = 0;
            loop {
                // Increment the probe and capture max Y coordinate, if new
                step(&mut probe);
                max_y = std::cmp::max(max_y, probe.y);

                // Check if we hit the target. If so, we're done for this init velocity
                if in_target(&target, &probe) {
                    overall_max = std::cmp::max(overall_max, max_y);
                    target_count += 1;
                    break;
                }

                // Bail out of this velocity after an arbitrary # of steps
                step_num += 1;
                if step_num > 1000 { break; }
            }
        }
    }

    println!("Part 1: Max Y height: {}\nPart 2: Target hit count: {}", overall_max, target_count);
}
