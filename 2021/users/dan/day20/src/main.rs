use std::fmt;
use std::fs;
use std::env;

#[derive(Default)]
struct Grid {
    data: Vec<char>,
    row_size: usize,
    row_count: usize,
}
impl Grid {
    fn new() -> Grid {
        Default::default()
    } 

    // Clones a grid, taking a function to determine how to fill out values
    // Adds num_pixels all around the original grid
    // Fills new pixels with character specified by 'e' param
    fn clone_extended<F>(&self, num_pixels: usize, e: char, lookup: F) -> Grid where
        // Function - Takes an x,y coordinate. Returns a character
        F: Fn(usize, usize) -> char {

        let mut grid_e = Grid{
            data: Vec::default(),
            row_size: self.row_size + num_pixels*2,
            row_count: self.row_count + num_pixels*2  };

        // Push in new rows of empty
        for _ in 0..num_pixels {
            for _ in 0..grid_e.row_size {
                grid_e.data.push(e);
            }
        }
    
        // Copy in the main part, with new columns of empty on either side
        for y in 0..self.row_count {
            for _ in 0..num_pixels {
                grid_e.data.push(e);
            }
    
            for x in 0..self.row_size {
                grid_e.data.push(lookup(x, y));
            }
    
            for _ in 0..num_pixels {
                grid_e.data.push(e);
            }
        }
    
        // Add more rows of empty
        for _ in 0..num_pixels {
            for _ in 0..grid_e.row_size {
                grid_e.data.push(e);
            }
        }

        grid_e
    }

    // Returns the value of a pixel at the given location
    // Assumes all pixels outside bounds match the pixel on an edge (0,0)
    fn get_pixel(&self, x: usize, y: usize) -> usize {
        let check = |x,y| {
            match self.data[y*self.row_size + x] {
                '#' => { return 1 },
                '.' => { return 0 },
                _ => { panic!("Invalid char in grid!"); }
            }
        };

        if x >= self.row_size {
            return check(0, 0);
        }

        if y >= self.row_count {
            return check(0, 0);
        }

        check(x, y)
    }

    // Calculate next pixel by checking the 9 pixels centered in the x,y coordinate
    // Accumulates a 9-bit number from those pixels, and uses that to look up in the lookup table
    fn next_pixel(&self, x: usize, y: usize, lookup: &Vec<char>) -> char {
        let y_i = y as isize;
        let x_i = x as isize;
        let lookup_idx = (y_i-1..y_i+2).fold(0, |y_sum,y_idx| {
            (x_i-1..x_i+2).fold(y_sum, |x_sum,x_idx| {
                let pixel = self.get_pixel(x_idx as usize, y_idx as usize);
                //println!("({},{})={} -> {}", x_idx, y_idx, pixel, x_sum);
                (2*x_sum) + pixel
            })
        });


        lookup[lookup_idx]
    }
}
impl fmt::Display for Grid {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let mut row = 0;
        loop {
            for i in 0..self.row_size {
                write!(f, "{}", self.data[i+(row*self.row_size)])?;
            }
            row += 1;
            write!(f, "\n")?;

            if row*self.row_size >= self.data.len() { break; }
        }
        Ok(())
    }
}


fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");

    let mut lines = input.lines();
    let lookup_table: Vec<char> = lines.next().unwrap()
        .chars().map(|x| x).collect();

    lines.next(); // Toss a line

    let mut grid = Grid::new();
    // Capture the initial image
    while let Some(line) = lines.next() {
        line.chars().for_each(|x| {
            grid.data.push(x);
        });
        if grid.row_size == 0 {
            grid.row_size = grid.data.len();
        }
        grid.row_count += 1;
    }

    // Clone the initial image to give additional space around the outside
    let clone_closure = |x,y| grid.data[y*grid.row_size + x];
    let mut grid_e = grid.clone_extended(3, '.', &clone_closure);

    println!("{}", lookup_table.iter().fold(String::new(), |sum,x| sum + &x.to_string()));
    println!("\n{}", grid_e);

    // Remap every pixel of the input image
    for _ in 0..2 {
        // There's a border of 3,3 around the image, so we can look up based on a pixel
        // within that border to figure out the new border
        let new_char = grid_e.next_pixel(1, 1, &lookup_table);
        let lookup_closure = |x,y| grid_e.next_pixel(x, y, &lookup_table);
        grid_e = grid_e.clone_extended(1, new_char, &lookup_closure);
        //println!("{}", grid_e);
    }

    // Count the number of lit pixels
    let lit_count = |grid: &Grid| {
        grid.data.iter().fold(0, |sum,c| {
            match c {
                '#' => { sum + 1 },
                '.' => { sum },
                _ => { panic!("Unknown character!"); }
            }
        })
    };

    println!("Part 1: Lit count: {}", lit_count(&grid_e));

    // Part 2 - enhance remaining # of times
    for _ in 0..48 {
        // There's a border of 3,3 around the image, so we can look up based on a pixel
        // within that border to figure out the new border
        let new_char = grid_e.next_pixel(1, 1, &lookup_table);
        let lookup_closure = |x,y| grid_e.next_pixel(x, y, &lookup_table);
        grid_e = grid_e.clone_extended(1, new_char, &lookup_closure);
        //println!("{}", grid_e);
    }
    
    println!("Part 2: Lit count: {}", lit_count(&grid_e));
}

#[cfg(test)]
mod tests{
   use super::*; 

    #[test]
    fn test_enhance_1() {
        let mut lookup_table = Vec::<char>::new();
        
        for c in "..#.#..#####.#.#.#.###.##.....###.##.#..###.####..#####..#....#..#..##..###..######.###...####..#..#####..##..#.#####...##.#.#..#.##..#.#......#.###.######.###.####...#.##.##..#..#..#####.....#.#....###..#.##......#.....#..#..#..##..#...##.######.####.####.#.#...#.......#..#.#.#...####.##.#......#..#...##.#.##..#...##.#.##..###.#......#.#.......#.#.#.####.###.##...#.....####.#..#..#.##.#....##..#.####....##...##..#...#......#.#.......#.......##..####..#...#.#.#...##..#.#..###..#####........#..####......#..#".chars() {
            lookup_table.push(c);
        }
        
        let mut grid = Grid::new();
        grid.row_size = 7;
        grid.row_count = 7;
        
        for c in ".......#..#...#......##..#....#......###.......".chars() {
            grid.data.push(c);
        }

        assert_eq!(grid.next_pixel(0, 0, &lookup_table), '.');
        assert_eq!(grid.next_pixel(1, 0, &lookup_table), '.');
        assert_eq!(grid.next_pixel(2, 0, &lookup_table), '#');
        assert_eq!(grid.next_pixel(3, 0, &lookup_table), '.');
        assert_eq!(grid.next_pixel(4, 0, &lookup_table), '#');
        assert_eq!(grid.next_pixel(2, 2, &lookup_table), '#');
    }
}