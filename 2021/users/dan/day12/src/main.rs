use std::fs;
use std::env;
use std::fmt;
use std::collections::hash_map::Entry;
use std::collections::HashMap;
use std::collections::HashSet;

#[derive(Clone)]
struct Cave {
    name: String,
    max_visits: usize,
    neighbors: Vec<String>
}

impl fmt::Display for Cave {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}: {}/{}, {}\n", self.name, self.visits, self.max_visits,
            self.neighbors.iter().fold(String::new(), |sum,x| sum + x + ","))?;

        Ok(())
    }
}

#[derive(Clone)]
struct CaveList {
    data: Vec<String>
}

impl fmt::Display for CaveList {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}", self.data.iter().fold(String::new(), |sum,x| sum + x + ","))?;

        Ok(())
    }
}

// Appends a cave to our map
fn add_cave(
    cave_system: &mut HashMap<String, Cave>,
    cave_name: &String,
    conn_cave: &String) {

    // Look up the cave in our map, either to update or to make new
    let cave = match cave_system.entry(cave_name.clone()) {
        Entry::Occupied(o) => o.into_mut(),
        Entry::Vacant(v) => v.insert(Cave{ 
            name: cave_name.clone(),
            visits: 0,
            max_visits: 
                // All-caps names have no visit limt
                if cave_name.chars().all(|x| x.is_uppercase()) { usize::MAX }
                else { 1 }, // Otherwise, only one visit allowed
                neighbors: Vec::<String>::default() })
    };

    // Append the connected cave to the neighbor list
    cave.neighbors.push(conn_cave.clone());
}

// Recursive function for visiting all caves
// Tracks progress by modifying the cave_system
// Next cave to visit is cave_name
// Returns the set of unique paths
fn visit(
    cave_system: &HashMap<String, Cave>,
    cave_name: &String,
    visit_list: &mut CaveList) -> HashSet<String> {

    // Put this cave in the visited list
    visit_list.data.push(cave_name.clone());

    let mut path_set = HashSet::<String>::new();

    // Check for base case (end of the line)
    if cave_name == "end" {
        // Flatten the path into a comma-separated string
        path_set.insert(visit_list.data.join(","));
        return path_set;
    }


    // Look up the  cave so we can modify its visit count
    let cave = cave_system.get(cave_name).unwrap();

    // Visit each neighbor.
    for n in &cave.neighbors {
        let max_visits: usize = cave_system.get(n).unwrap().max_visits;
        let visit_count: usize = visit_list.data.iter().filter(|x| x == &n).count();
        let mut visit_copy = visit_list.clone();

        if visit_count < max_visits {
            let paths = visit(&cave_system, &n, &mut visit_copy);
            path_set.extend(paths);
        }
    }

    return path_set;
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");

    let mut cave_system = HashMap::<String, Cave>::new();

    input.lines().for_each(|line| {
        let mut caves = line.split('-').map(|x| x.to_string());

        let one = caves.next().unwrap();
        let two = caves.next().unwrap();

        add_cave(&mut cave_system, &one, &two);
        add_cave(&mut cave_system, &two, &one);

    });

    println!("Caves:");
    for entry in cave_system.iter() {
        println!("{}", entry.1);
    }

    let part1_paths = visit(
        &cave_system,
        &"start".to_string(),
        &mut CaveList{ data: Vec::<String>::new() });
    //println!("Paths: {}", part1_paths.iter().fold(String::new(), |sum,x| sum + x + "\n"));
    println!("Part 1: path count = {}", part1_paths.len());

    // Part 2 - Allow up to 2 visits on one small cave
    // Clone the map for each small room, and modify it to allow 2 visits for that room
    let mut part2_paths  = HashSet::<String>::new();
    let cave_names: Vec<String> = cave_system.iter().map(|x| x.0.to_string()).collect();
    for cave_name in cave_names {
        let mut part2_cave_system = cave_system.clone();
        let cave: &mut Cave = part2_cave_system.get_mut(&cave_name).unwrap();

        if cave.max_visits == 1 && cave.name != "start" && cave.name != "end" {
            // Visit with this cave allowed up to 2
            cave.max_visits = 2;
        }
        else { continue; }

        println!("Visiting cave {} twice", cave.name);
        part2_paths.extend(visit(
            &part2_cave_system,
            &"start".to_string(),
            &mut CaveList{ data: Vec::<String>::new() }));
    }

    //println!("Paths: {}", part2_paths.iter().fold(String::new(), |sum,x| sum + x + "\n"));
    println!("Part 2: path count = {}", part2_paths.len());
}
