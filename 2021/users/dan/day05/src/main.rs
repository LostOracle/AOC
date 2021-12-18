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
}

impl PartialEq for GridRef {
    fn eq(&self, other: &Self) -> bool {
        self.x == other.x
            && self.y == other.y
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

struct Line {
    start: GridRef,
    stop: GridRef
}

impl Grid {
    fn mark_straight(&mut self, line: &Line) {
        // Figure out if we have a horizontal or vertical line
        match line.start.x == line.stop.x {
            true => {
                let stop = std::cmp::max(line.start.y, line.stop.y);
                let start = std::cmp::min(line.start.y, line.stop.y);
                for y in start..stop+1 {
                    self.data[y][line.start.x] += 1;
                }
            }
            false => {
                let stop = std::cmp::max(line.start.x, line.stop.x);
                let start = std::cmp::min(line.start.x, line.stop.x);
                for x in start..stop+1 {
                    self.data[line.start.y][x] += 1;
                }
            }
        }
    }

    fn mark_diag(&mut self, line: &Line) {
        // Figure out if we are angled up or angled down
        let xdiff = (line.start.x - line.stop.x) as isize;
        let ydiff = (line.start.y - line.stop.y) as isize;
        let down_right = (xdiff > 0 && ydiff > 0)
            || (xdiff < 0 && ydiff < 0);
        let stop_y = std::cmp::max(line.start.y, line.stop.y);
        let start_y = std::cmp::min(line.start.y, line.stop.y);
        match down_right {
            true => {
                let mut curr_x = std::cmp::min(line.start.x, line.stop.x);
                for y in start_y..stop_y+1 {
                    self.data[y][curr_x] += 1;
                    curr_x += 1;
                }
            }
            false => {
                let mut curr_x = std::cmp::max(line.start.x, line.stop.x);
                for y in start_y..stop_y+1 {
                    self.data[y][curr_x] += 1;
                    curr_x -= 1;
                }
            }
        }
    }
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");

    let line_spec: Vec<Line> = input.lines().map(|line| {
        let coords: Vec<&str> = line
            .split(&[',', '-', '>'][..])
            .filter(|x| x.len() > 0)
            .collect();

        let mut coords_iter = coords.iter();

        Line{
            start: GridRef{
                x: coords_iter.next().unwrap().trim().parse::<usize>().unwrap(),
                y: coords_iter.next().unwrap().trim().parse::<usize>().unwrap() },
            stop: GridRef {
                x: coords_iter.next().unwrap().trim().parse::<usize>().unwrap(),
                y: coords_iter.next().unwrap().trim().parse::<usize>().unwrap() },
        }
    }).collect();

    for l in &line_spec {
        println!("{},{} -> {},{}", l.start.x, l.start.y, l.stop.x, l.stop.y);
    }

    use std::iter::once;

    // Figure out the grid size and initialize
    let max_x = line_spec.iter()
        .map(|line| (line.start.x, line.stop.x))
        .flat_map(|tup| once(tup.0).chain(once(tup.1)))
        .max().unwrap();

    let max_y = line_spec.iter()
        .map(|line| (line.start.y, line.stop.y))
        .flat_map(|tup| once(tup.0).chain(once(tup.1)))
        .max().unwrap();

    let max_x_vec: Vec<u8> = vec![0; max_x+1];
    let mut grid = Grid::default();
    for _ in 0..max_y+1 {
        grid.data.push(max_x_vec.clone());
    }
    //println!("Max X = {}, Max Y = {}", max_x, max_y);

    // Part 1 - select all vertical and horizontal lines
    // Mark those lines on the grid
    line_spec.iter()
        .filter(|line| line.start.x == line.stop.x || line.start.y == line.stop.y)
        .for_each(|cl| { grid.mark_straight(cl); } );

    // Count all the intersections
    let mut count = 0;
    for y in 0..grid.data.len() {
        for x in 0..grid.data[0].len() {
            if grid.data[y][x] > 1 {
                count += 1;
            }
        }
    }
    //println!("Grid: \n{}", grid);
    println!("Part 1: {}", count);

    // Part 2 - mark diag lines
    line_spec.iter()
        .filter(|line| line.start.x != line.stop.x && line.start.y != line.stop.y)
        .for_each(|cl| { grid.mark_diag(cl); } );

    // Count all the intersections
    let mut count = 0;
    for y in 0..grid.data.len() {
        for x in 0..grid.data[0].len() {
            if grid.data[y][x] > 1 {
                count += 1;
            }
        }
    }
    //println!("Grid: \n{}", grid);
    println!("Part 2: {}", count);
}
