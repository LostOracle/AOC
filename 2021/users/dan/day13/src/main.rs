use std::fs;
use std::env;
use std::fmt;

#[derive(Default)]
#[derive(Ord,Eq)]
#[derive(PartialEq,PartialOrd)]
struct Coord {
    y: isize,
    x: isize,
}

#[derive(Default)]
struct Grid {
    dot_list: Vec<Coord>
}

impl fmt::Display for Grid {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        // Print out the pattern
        let x_max = self.dot_list.iter().map(|coord| coord.x).max().unwrap();
        let y_max = self.dot_list.iter().map(|coord| coord.y).max().unwrap();

        //println!("X max: {}, Y max: {}", x_max, y_max);
        for y in 0..y_max+1 {
            for x in 0..x_max+1 {
                // Determine if we should display '#' or '.'
                let is_filled = self.dot_list.iter()
                    .find(|c| c.y == y && c.x == x);
                match is_filled {
                    Some(_) => { write!(f, "#")?; },
                    None => { write!(f, ".")?; }
                }
            }
            print!("\n");
        }

        Ok(())
    }
}

impl Grid {
    fn fold_x(&mut self, x: isize) {
        // For each coordinate, reflect on the x axis
        // For x, we fold "left", which means we locate dots to the right of the fold
        // and move them left by 2x abs dist from the fold
        self.dot_list.iter_mut()
            .filter(|dot| dot.x > x)
            .for_each(|dot| {
                dot.x -= (dot.x - x).abs() * 2;
            });

        // Now check for duplicates and remove them
        self.dot_list.sort();
        self.dot_list.dedup();
    }

    fn fold_y(&mut self, y: isize) {
        // For each coordinate, reflect on the y axis
        // For y, we fold "up", which means we locate dots below the fold
        // and move them up by 2x abs dist from the fold
        self.dot_list.iter_mut()
            .filter(|dot| dot.y > y)
            .for_each(|dot| {
                dot.y -= (dot.y - y).abs() * 2;
            });

        // Now check for duplicates and remove them
        self.dot_list.sort();
        self.dot_list.dedup();
    }
}

struct Fold {
    axis: char,
    idx: isize
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");

    let mut grid = Grid::default();

    let mut lines = input.lines();

    // Read in the dot locations
    loop {
        let line = lines.next().unwrap();
        if !line.contains(',') { break; }

        let mut line = line.split(',');

        let c = Coord{
            x: line.next().unwrap().parse::<isize>().unwrap(),
            y: line.next().unwrap().parse::<isize>().unwrap() };

        grid.dot_list.push(c);
    }

    // Read in the fold instructions
    let mut instructions = Vec::<Fold>::new();
    lines.for_each(|line| {
        let mut inst = line.split('=');
        let dir: char = inst.next().unwrap().chars().last().unwrap();
        let val: isize = inst.next().unwrap().parse::<isize>().unwrap();

        instructions.push(Fold{ axis: dir, idx: val });
    });

    println!("Coords: \n{}", grid.dot_list.iter().fold(String::new(),
        |sum,coord| sum + &coord.x.to_string() + "," + &coord.y.to_string() + "\n"));

    println!("Instructions: \n{}", instructions.iter().fold(String::new(),
        |sum,inst| sum + "fold along " + &inst.axis.to_string() + "=" + &inst.idx.to_string() + "\n"));
    //println!("Grid: \n{}", grid);

    // Execute first fold instruction
    let inst_head = instructions.iter().next().unwrap();
    match inst_head.axis {
        'y' => grid.fold_y(inst_head.idx),
        'x' => grid.fold_x(inst_head.idx),
        _ => panic!("Invalid fold axis: {}", inst_head.axis)
    }

    // Count dots
    println!("Part 1: Coord count = {}", grid.dot_list.len());

    // Execute all remaining folds
    let mut inst_tail = instructions.iter();
    inst_tail.next(); // Toss the first one

    for inst in inst_tail {
        match inst.axis {
            'y' => grid.fold_y(inst.idx),
            'x' => grid.fold_x(inst.idx),
            _ => panic!("Invalid fold axis: {}", inst.axis)
        }
    }

    println!("Part 2: Grid: \n{}", grid);

}
