use std::fs;
use std::env;
use std::fmt;

#[derive(Clone)]
#[derive(Copy)]
#[derive(Default)]
struct Octo {
    flashed: bool,
    energy: u8
}

#[derive(Clone)]
#[derive(Default)]
struct Grid {
    data: Vec<Vec<Octo>>,
}

impl fmt::Display for Grid {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        for row in &self.data {
            write!(f, "{}\n", row.iter().fold(
                String::new(), |sum, x| sum + &x.energy.to_string()))?;
        }

        Ok(())
    }
}

impl Grid {
    // Flashes the octopus at (x,y) in the grid
    fn flash(&mut self, x: usize, y: usize) {
        self.data[y][x].flashed = true;
        
        // Iterate through the neighbors of the grid entry
        // Note that we use -1..2 because "end" is excluded from the Range
        for i in (-1 as isize)..2 {
            for j in (-1 as isize)..2 {
                if i == 0 && j == 0 { continue; }

                // We need to preserve signed to check bounds
                let new_x = x as isize + i;
                let new_y = y as isize + j;

                // Indices must be unsigned
                let x_idx = new_x as usize;
                let y_idx = new_y as usize;

                // Check bounds of the index vars before use
                if new_x >= 0 && new_y >= 0
                    && x_idx < self.data[y].len()
                    && y_idx < self.data.len() {
                    // Increment energy of the octopus at this location
                    self.data[y_idx][x_idx].energy += 1;
                }
            }
        }
    }

    // Increments the simulation by one step
    // Returns # of octopi who flashed
    fn step(&mut self) -> u64 {

        // Increase all energy by 1
        self.data.iter_mut().for_each(|line| {
            line.iter_mut().for_each(|octo| { octo.energy += 1; } );
        });

        // Clean count indicates if no flashes occurred
        let mut clean_count = 0;
        loop {
            // Check for >9 until all octupi have flashed, or no >9 exists
            for y in 0..self.data.len() {
                for x in 0..self.data[0].len() {
                    let octo = &self.data[y][x];
                    if octo.energy > 9 && !octo.flashed {
                        self.flash(x, y);
                        clean_count = 0;
                    }
                }
            }
            clean_count += 1;
            // Need to have 2 clean runs to ensure we're stable
            if clean_count > 1 { break; }
        }

        // Reset all flashers
        let mut total_flashers = 0;
        for y in 0..self.data.len() {
            for x in 0..self.data[0].len() {
                let octo = &mut self.data[y][x];
                if octo.flashed {
                    octo.flashed = false;
                    octo.energy = 0;
                    total_flashers += 1;
                }
            }
        }

        total_flashers
    }
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");


    let mut grid = Grid::default();
    input.lines().for_each(|line| {
        let l = line.chars()
            .map(|c| Octo{
                flashed: false,
                energy: c.to_digit(10).unwrap() as u8 })
            .collect();

        grid.data.push(l);
    });

    println!("Grid: \n{}", grid);

    //for i in 0..10 {
    //    grid.step();
    //    println!("After iteration {}:\n{}", i+1, grid);
    //}

    // Clone the grid for part 2
    let mut part2_grid = grid.clone();

    // Step 100 times
    let total_flashes = (0..100).fold(0, |acc,_| acc + grid.step());
    println!("Part 1: Flashes after 100 steps = {}", total_flashes);

    // Part 2 - iterate until all octupi flash in the same step
    let max_octopi = (grid.data[0].len() * grid.data.len()) as u64;
    let mut iter = 0;
    loop {
        iter += 1;
        let curr_flashes = part2_grid.step();
        if curr_flashes == max_octopi { break; }
    }

    println!("Part 2: Cycle with all flash = {}", iter);
}
