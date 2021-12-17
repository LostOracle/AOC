use std::fs;
use std::env;
use std::fmt;
use std::collections::BinaryHeap;
use std::collections::VecDeque;
use std::cmp::Ordering;
use cpuprofiler::PROFILER;

#[derive(Clone)]
#[derive(Default)]
struct Grid {
    data: Vec<Vec<u8>>,
}

#[derive(Clone)]
#[derive(Copy)]
#[derive(Default)]
#[derive(Eq)]
struct GridRef {
    cost: u8,
    x: usize,
    y: usize,
}

// Tracks per-node information for our search
#[derive(Clone)]
struct NodeScore {
    g: usize, // score to this point
    f: usize, // estimate of score to end
    prev: GridRef, // best previous node
}

impl Default for NodeScore {
    fn default() -> Self {
        NodeScore{ g: usize::MAX, f: usize::MAX, prev: GridRef::default() }
    }
}


impl PartialEq for GridRef {
    fn eq(&self, other: &Self) -> bool {
        self.x == other.x
            && self.y == other.y
            && self.cost == other.cost
    }
}

// Impl ord so we can have a sorted heap
impl Ord for GridRef {
    fn cmp(&self, other: &Self) -> Ordering {
        other.cost.cmp(&self.cost)
            .then_with(|| self.x.cmp(&other.x))
            .then_with(|| self.y.cmp(&other.y))
    }
}

impl PartialOrd for GridRef {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
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
    fn get_ref(&self, x: usize, y: usize) -> GridRef {
        GridRef{ x: x, y: y, cost: self.data[y][x] }
    }

    fn adjacent(&self, x: usize, y: usize, adj: &mut Vec<GridRef>) {
        adj.clear();

        // Check all orthogonal indices
        if x > 0 {
            adj.push(GridRef{ x: x-1, y: y, cost: self.data[y][x-1] });
        }

        if x < self.data[y].len() - 1 {
            adj.push(GridRef{ x: x+1, y: y, cost: self.data[y][x+1] });
        }

        if y > 0 {
            adj.push(GridRef{ x: x, y: y-1, cost: self.data[y-1][x] });
        }

        if y < self.data.len() - 1 {
            adj.push(GridRef{ x: x, y: y+1, cost: self.data[y+1][x] });
        }
    }

    fn to_idx(&self, gr: GridRef) -> usize {
        gr.x + gr.y * self.data[0].len()
    }

    // Heuristic function for estimating distance to goal
    // We'll calculate the manhattan distance
    fn h(&self, pos: GridRef, goal: GridRef) -> usize {
        (goal.x - pos.x) + (goal.y - pos.y)
    }

    // Returns total cost of route
    fn astar(&self, start: GridRef, goal: GridRef) -> usize {
        // Holds nodes we wish to explore next
        let mut open_nodes = BinaryHeap::<GridRef>::new();
        open_nodes.push(start);

        let mut node_scores = Vec::<NodeScore>::new();
        node_scores.resize(self.data.len() * self.data[0].len(), NodeScore::default());

        // Initialize the start node info
        node_scores[self.to_idx(start)] = NodeScore{
            g: 0,
            f: self.h(start, goal),
            prev: GridRef{ x: usize::MAX, y: usize::MAX, cost: 0 }};

        // Temp variable to store adjacent nodes
        let mut adj_nodes = Vec::<GridRef>::new();

        // Loop until we've explored everything or reached the end
        while let Some(current) = open_nodes.pop() {
            //println!("Checking {},{}", current.x, current.y);

            // Check thes core of each adjacent node
            self.adjacent(current.x, current.y, &mut adj_nodes);
            for neigh in &adj_nodes {
                let current_g_score = node_scores[self.to_idx(current)].g;

                // Initialize node_scores if needed
                let neigh_score = &mut node_scores[self.to_idx(*neigh)];
                if neigh_score.g == usize::MAX {
                    neigh_score.f = self.h(start, goal);
                    neigh_score.prev = current;
                }
                
                // Calculate next cost for using this neighbor
                let next_cost: usize = self.data[neigh.y][neigh.x] as usize;
                let tentative_g_score = current_g_score + next_cost;

                // If this neighbor is the cheapest so far, use it instead
                if tentative_g_score < neigh_score.g {
                    neigh_score.prev = current;
                    neigh_score.g = tentative_g_score;
                    neigh_score.f = tentative_g_score + self.h(*neigh, goal);

                    // There's a possible improvement here, but it involves checking existence
                    // of the neighbor in our prioritized list before pushing
                    // Problem is, it's O(n) to verify neighbor isn't in the priority queue
                    // So we'll just push all the time instead.
                    open_nodes.push(*neigh);
                    //println!("--Added {},{}", neigh.x, neigh.y);
                }
            }

        }

        // Reproduce the path by tracing backwards
        //let mut path = VecDeque::<GridRef>::new();
        //let mut current = goal;
        //while current.x != usize::MAX && current.y != usize::MAX {
        //    path.push_front(current);
        //    current = node_scores.get(&current).unwrap().prev;
        //}

        //for p in path {
        //    println!("{},{}", p.x, p.y);
        //}

        node_scores[self.to_idx(goal)].f
    }

}

fn inc_wrap(val: u8, inc: u8) -> u8 {
    if val + inc > 9 { 1 + ((val + inc) % 10) }
    else { (val + inc) % 10 }
}

fn multiply(grid: Grid) -> Grid {
    let orig_grid = grid.clone();

    let mut new_grid = Grid::default();

    for ver in 0..5 {
        for row in &orig_grid.data {
            let mut new_row = Vec::<u8>::new();
            for hor in 0..5 {
                for col in row {
                    new_row.push(inc_wrap(*col, ver+hor));
                }
            }
            new_grid.data.push(new_row);
        }
    }

    //println!("New grid: \n{}", new_grid);

    return new_grid;
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

    //println!("Grid: \n{}", grid);

    // Using a profiler to optimize performance
    // (original versions of this problem were very slow)
    PROFILER.lock().unwrap().start("./my-prof.profile");

    // Explore the grid for best path
    let start = grid.get_ref(0, 0);
    let goal = grid.get_ref(grid.data[0].len()-1, grid.data.len()-1);
    let cost = grid.astar(start, goal);

    println!("Part 1 cost: {}", cost);

    // Multiply the grid and run it again
    let big_grid = multiply(grid);
    let start = big_grid.get_ref(0, 0);
    let goal = big_grid.get_ref(big_grid.data[0].len()-1, big_grid.data.len()-1);
    let cost = big_grid.astar(start, goal);

    println!("Part 2 cost: {}", cost);
    PROFILER.lock().unwrap().stop();
}
