use std::fs;
use std::env;
use std::fmt;

#[derive(Default)]
struct Grid {
    data: Vec<Vec<u8>>,
}

#[derive(Clone)]
#[derive(Copy)]
#[derive(Default)]
struct GridRef {
    x: usize,
    y: usize,
    val: u8
}

impl PartialEq for GridRef {
    fn eq(&self, other: &Self) -> bool {
        self.x == other.x
            && self.y == other.y
            && self.val == other.val
    }
}

impl fmt::Display for Grid {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        for row in &self.data {
            write!(f, "{}\n", row.iter().fold(
                String::new(), |sum, x| sum + &x.to_string()))?;
        }

        Ok(())
    }
}

impl Grid {
    fn adjacent(&self, x: usize, y: usize) -> Vec<GridRef> {
        let mut adj = Vec::<GridRef>::new();

        // Check all orthogonal indices
        if x > 0 {
            adj.push(GridRef{ x: x-1, y: y, val: self.data[y][x-1] });
        }

        if x < self.data[y].len() - 1 {
            adj.push(GridRef{ x: x+1, y: y, val: self.data[y][x+1] });
        }

        if y > 0 {
            adj.push(GridRef{ x: x, y: y-1, val: self.data[y-1][x] });
        }

        if y < self.data.len() - 1 {
            adj.push(GridRef{ x: x, y: y+1, val: self.data[y+1][x] });
        }

        adj
    }
}

fn calc_basin_size(grid: &Grid, min: GridRef) -> u64 {

    // Push minimum onto stack
    let mut stack = Vec::<GridRef>::new();
    stack.push(min);

    // Keep track of grid locations we've visited so we don't double-count
    let mut visited = Vec::<GridRef>::new();
    visited.push(min);

    // Pop item off stack. Add adjacent to stack if not 9 and not already visited
    // Continue until stack is empty
    let mut basin_size: u64 = 0;
    while stack.len() > 0 {
        let entry = stack.pop().unwrap();
        basin_size += 1;

        grid.adjacent(entry.x, entry.y).iter()
            .for_each(|a| {
                if a.val != 9 && !visited.contains(&a) {
                    visited.push(*a);
                    stack.push(*a);
                }
            });

    }

    return basin_size;
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");


    let mut grid = Grid::default();
    input.lines().for_each(|line| {
        let l = line.chars().map(|c| c.to_digit(10).unwrap() as u8).collect();

        grid.data.push(l);
    });

    println!("Grid: \n{}", grid);

    let mut minimums = Vec::<GridRef>::new();

    // Now find the lowest adjacent points in the grid
    let risk_level = grid.data.iter().enumerate().map(|row| {
        row.1.iter().enumerate().map(|col| {
            let adj = grid.adjacent(col.0, row.0);
            if adj.iter().all(|entry| entry.val > *col.1) { 
                minimums.push(GridRef{ x: col.0, y: row.0, val: *col.1 });
                (col.1 + 1) as u64
            }
            else { 0 as u64 }
        }).sum::<u64>()
    }).sum::<u64>();

    println!("Part 1 - Risk level: {}", risk_level);

    // Part 2 - calculate basin sizes
    let mut basins = Vec::<u64>::new();

    for min in minimums.drain(..) {
        basins.push(calc_basin_size(&grid, min));
    }

    basins.sort_by(|a,b| b.cmp(a));
    let top_three: u64 = basins.iter().take(3).fold(1, |prod,x| prod * x);

    println!("Part 2 - Top 3 basins: {} ({})", top_three,
        basins.iter().take(3).fold("".to_string(), |s,x| s + &x.to_string() + ", "));
}
