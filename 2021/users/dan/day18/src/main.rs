use std::fs;
use std::fmt;
use std::env;

struct Tree {
    l: Option<Box<NodeType>>,
    r: Option<Box<NodeType>>,
}

// Each node is either another tree, or a single value
enum NodeType {
    Child(Box<Tree>),
    Val(u32)
}

struct ExplodeUp {
    l: u32,
    r: u32,
    boom: bool, // Indicates we have exploded
    first: bool, // Indicates this is the first up-call from an explode
    came_left: bool // Indicates we exploded on the left side of the tree
}

fn is_leaf(n: &NodeType) -> bool {
    match n {
        NodeType::Child(_) => { false },
        NodeType::Val(_) => { true }
    }
}

impl Tree {
    fn explode_check(&mut self, depth: usize) -> ExplodeUp {
        let mut exploded = ExplodeUp{ l: 0, r: 0, boom: false, first: false, came_left: false};

        let l_leaf = match &self.l { Some(t) => { is_leaf(&t) }, None => { false }};
        let r_leaf = match &self.r { Some(t) => { is_leaf(&t) }, None => { false }};
        if l_leaf && r_leaf && depth > 4 {
            // Explode!
            exploded.l = match **self.l.as_ref().unwrap() { NodeType::Val(v) => { v }, _ => { 0 }, };
            exploded.r = match **self.r.as_ref().unwrap() { NodeType::Val(v) => { v }, _ => { 0 }, };
            exploded.boom = true;
            exploded.first = true;
        }
        else {
            match &mut self.l {
                Some(t) => {
                    let node: &mut NodeType = &mut **t;
                    match node {
                        NodeType::Child(c) => {
                            if !exploded.boom {
                                exploded = c.explode_check(depth+1);
                            }
                            match &mut **self.r.as_mut().unwrap() {
                                NodeType::Child(_) => {},
                                NodeType::Val(v) => {
                                    *v += exploded.r;
                                    exploded.r = 0;
                                }
                            }

                            if exploded.first {
                                *node = NodeType::Val(0);
                                exploded.first = false;
                            }
                            
                            exploded.came_left = true;
                        },
                        NodeType::Val(_) => {}
                    }
                },
                None => {} }
            match &mut self.r {
                Some(t) => {
                    let node: &mut NodeType = &mut **t;
                    match node {
                        NodeType::Child(c) => {
                            if !exploded.boom {
                                exploded = c.explode_check(depth+1);
                            }
                            match &mut **self.l.as_mut().unwrap() {
                                NodeType::Child(_) => {},
                                NodeType::Val(v) => {
                                    *v += exploded.l;
                                    exploded.l = 0;
                                }
                            }

                            if exploded.first {
                                *node = NodeType::Val(0);
                                exploded.first = false;
                            }

                            exploded.came_left = false;
                        },
                        NodeType::Val(_) => {}
                    }
                },
                None => {}
            }
        }

//        if depth == 1 {
//            if exploded.l > 0 && !exploded.came_left {
//                self.carry(exploded.l);
//            }
//
//            if exploded.r > 0 && exploded.came_left {
//                self.carry(exploded.r);
//            }
//        }


        println!("Explode check depth {} => {}", depth, exploded.boom);
        exploded
    }

    fn split_check(&mut self) -> bool {
        let mut found = false;
        match &mut self.l { 
            Some(t) => { 
                let node: &mut NodeType = &mut **t;
                found = match node {
                NodeType::Child(c) => { c.split_check() },
                NodeType::Val(v) => {
                    if *v >= 10 {
                        let left = Box::new( NodeType::Val( (*v as f32 / 2 as f32).floor() as u32 ) );
                        let right = Box::new( NodeType::Val( (*v as f32 / 2 as f32).ceil() as u32 ) );
                        let new_tree = Tree{ l: Some(left), r: Some(right) };
                        *node = NodeType::Child( Box::new(new_tree) );

                        true
                    }
                    else
                    {
                        false
                    }
                }
            }},
            None => {}
        }

        if !found {
            match &mut self.r {
                Some(t) => {
                    let node: &mut NodeType = &mut **t;
                    found = match node {
                    NodeType::Child(c) => { c.split_check() },
                    NodeType::Val(v) => {
                        if *v >= 10 {
                            let left = Box::new( NodeType::Val( (*v as f32 / 2 as f32).floor() as u32 ) );
                            let right = Box::new( NodeType::Val( (*v as f32 / 2 as f32).ceil() as u32 ) );
                            let new_tree = Tree{ l: Some(left), r: Some(right) };
                            *node = NodeType::Child( Box::new(new_tree) );
    
                            true
                        }
                        else
                        {
                            false
                        }
                    }}
                },
                None => {}
            }
        }

        found
    }

    // Push downwards into the tree to carry in explodes
    fn carry(&mut self, carry_val: u32) -> bool {
        let mut found = false;

        if let Some(t) = &mut self.l {
            found = match &mut **t {
                NodeType::Child(c) => { c.carry(carry_val) },
                NodeType::Val(v) => {
                    *v += carry_val;
                    true
                }
            }
        }
        if !found {
            if let Some(t) = &mut self.r {
                found = match &mut **t {
                    NodeType::Child(c) => { c.carry(carry_val) },
                    NodeType::Val(v) => {
                        *v += carry_val;
                        true
                    }
                }
            }
        }

        found
    }
}

impl fmt::Display for Tree {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "[{},{}]", match &self.l {
            Some(t) => { t },
            None => { panic!("can't print a none"); }
        }, match &self.r {
            Some(t) => { t },
            None => { panic!("can't print a none"); }
        })
    }
}

impl fmt::Display for NodeType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match &self {
            NodeType::Child(c) => { write!(f, "{}", c) },
            NodeType::Val(v) => { write!(f, "{}", v.to_string()) }
        }
    }
}

// Returns number of characters handled
fn fill_node(s: &str, n: &mut NodeType) -> usize {
    let mut idx: usize = 1;
    
    let c: char = s.chars().nth(0).unwrap();
    match c {
        '[' => {
            let mut t = Tree{ l: None, r: None };
            idx += from_string(&s[1..], &mut t);
            *n = NodeType::Child( Box::new(t) );
        },
        '0' | '1' | '2' |'3' | '4' | '5' | '6' | '7' | '8' | '9' => {
            *n = NodeType::Val(c.to_digit(10).unwrap());
        },
        ',' => {},
        ']' => { /* Time to return */ },
        _ => { println!("Invalid char: {}", c); panic!("Aborting.") }
    }

    idx
}

// Parses a string of [1,2],3] into a tree (note, first [ is removed)
// Returns number of characters handled
fn from_string(s: &str, t: &mut Tree) -> usize {
    let mut idx: usize = 0;

    let mut n = NodeType::Val(0);
    idx += fill_node(&s[0..], &mut n);
    t.l = Some(Box::new(n));
    let c: char = s.chars().nth(idx).unwrap();
    match c {
        ',' => {},
        _ => {
            println!("from_string - invalid char: {}", c);
            panic!("Aborting"); }
    }
    idx += 1;
    let mut n = NodeType::Val(0);
    idx += fill_node(&s[idx..], &mut n);
    t.r = Some(Box::new(n));
    idx += 1; // Remove closing bracket

    idx
}

fn magnitude_node(n: &NodeType, f: u32) -> u32 {
    match n {
        NodeType::Child(c) => {
            f * magnitude(&c)
        },
        NodeType::Val(v) => {
            f * v
        }
    }
}

fn magnitude(t: &Tree) -> u32 {
    let mut sum = 0;

    sum += match &t.l {
        Some(s) => { magnitude_node(&s, 3) },
        None => { 0 },
    };
    sum += match &t.r {
        Some(s) => { magnitude_node(&s, 2) },
        None => { 0 },
    };

    sum
}

fn reduce(t: &mut Tree) {

    loop {
        let mut action_taken = t.explode_check(1).boom; // Check for explodes
        if !action_taken {
            action_taken = t.split_check(); // Check for splits
        }

        if !action_taken { break; }
    }
}

fn add(left: Tree, right: Tree) -> Tree {

    let mut t = Tree{
        l: Some(Box::new(NodeType::Child(Box::new(left)))),
        r: Some(Box::new(NodeType::Child(Box::new(right)))) };
    reduce(&mut t);
    t
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");

    // Process input into a list of "snailfish numbers"
    // Parse each number into a tree
    // Perform addition
    // Calculate magnitude of final sum
    let mut final_tree = Tree{ l: None, r: None };
    for line in input.lines() {
        let mut t = Tree{ l: None, r: None };
        from_string(&line[1..], &mut t);
        println!("Next Tree: {}", t);

        if let None = final_tree.l {
            final_tree = t;
        }
        else {
            final_tree = add(final_tree, t);
        }
        println!("Current: {}", final_tree);
    }

}

#[cfg(test)]
mod tests{
   use super::*; 

    #[test]
    fn test_mag_1() {
        let mut t = Tree{ l: None, r: None };
        from_string("9,1]", &mut t);
        assert_eq!(magnitude(&t), 29);
    }

    #[test]
    fn test_mag_2() {
        let mut t = Tree{ l: None, r: None };
        from_string("1,9]", &mut t);
        assert_eq!(magnitude(&t), 21);
    }

    #[test]
    fn test_mag_3() {
        let mut t = Tree{ l: None, r: None };
        from_string("[9,1],[1,9]]", &mut t);
        assert_eq!(magnitude(&t), 129);
    }

    #[test]
    fn test_mag_4() {
        let mut t = Tree{ l: None, r: None };
        from_string("[1,2],[[3,4],5]]", &mut t);
        assert_eq!(magnitude(&t), 143);
    }

    #[test]
    fn test_mag_5() {
        let mut t = Tree{ l: None, r: None };
        from_string("[[[0,7],4],[[7,8],[6,0]]],[8,1]]", &mut t);
        assert_eq!(magnitude(&t), 1384);
    }

    #[test]
    fn test_mag_6() {
        let mut t = Tree{ l: None, r: None };
        from_string("[[[1,1],[2,2]],[3,3]],[4,4]]", &mut t);
        assert_eq!(magnitude(&t), 445);
    }

    #[test]
    fn test_mag_7() {
        let mut t = Tree{ l: None, r: None };
        from_string("[[[3,0],[5,3]],[4,4]],[5,5]]", &mut t);
        assert_eq!(magnitude(&t), 791);
    }

    #[test]
    fn test_mag_8() {
        let mut t = Tree{ l: None, r: None };
        from_string("[[[5,0],[7,4]],[5,5]],[6,6]]", &mut t);
        assert_eq!(magnitude(&t), 1137);
    }

    #[test]
    fn test_mag_9() {
        let mut t = Tree{ l: None, r: None };
        from_string("[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]", &mut t);
        assert_eq!(magnitude(&t), 3488);
    }

    #[test]
    fn test_expl_1() {
        let mut t = Tree{ l: None, r: None };
        from_string("[[[[9,8],1],2],3],4]", &mut t);
        reduce(&mut t);
        assert_eq!(format!("{}", t), "[[[[0,9],2],3],4]");
    }

    #[test]
    fn test_expl_2() {
        let mut t = Tree{ l: None, r: None };
        from_string("7,[6,[5,[4,[3,2]]]]]", &mut t);
        reduce(&mut t);
        assert_eq!(format!("{}", t), "[7,[6,[5,[7,0]]]]");
    }

    #[test]
    fn test_expl_3() {
        let mut t = Tree{ l: None, r: None };
        from_string("[6,[5,[4,[3,2]]]],1]", &mut t);
        reduce(&mut t);
        assert_eq!(format!("{}", t), "[[6,[5,[7,0]]],3]");
    }

    #[test]
    fn test_expl_4() {
        let mut t = Tree{ l: None, r: None };
        from_string("[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]", &mut t);
        t.explode_check(1);
        assert_eq!(format!("{}", t), "[[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]");
    }

    #[test]
    fn test_expl_5() {
        let mut t = Tree{ l: None, r: None };
        from_string("[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]", &mut t);
        reduce(&mut t);
        assert_eq!(format!("{}", t), "[[3,[2,[8,0]]],[9,[5,[7,0]]]]");
    }
}