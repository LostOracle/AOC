use std::fs;
use std::convert::TryFrom;
use std::str::FromStr;
#[macro_use] extern crate scan_fmt;

#[derive (Debug, Copy, Clone)]
enum Instruction {
    Acc(i64),
    Jmp(i64),
    Nop(i64),
}

impl FromStr for Instruction {
    type Err = ();

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let (string_op, arg) = scan_fmt!(s, "{} {}", String, i64).unwrap();

        let op = match string_op.as_str() {
            "acc" => Instruction::Acc(arg),
            "jmp" => Instruction::Jmp(arg),
            "nop" => Instruction::Nop(arg),
            _ => panic!("Invalid Opcode"),
        };

        Ok( op )
    }
}

#[derive (Debug, Copy, Clone)]
struct State {
    acc: i64,
    pc: usize,
}

impl State {
    fn new() -> Self {
        State{ acc: 0, pc: 0 }
    }
}

#[derive (Debug)]
struct Computer {
    state: State,
    rom: Vec<Instruction>,
    history: Vec<State>,
}

impl Computer {
    fn new( rom: Vec<Instruction> ) -> Self {
        Self{ state: State::new(), rom, history: Vec::new() }
    }

    fn run(&mut self) -> i32 {
        loop {
            if self.state.pc >= self.rom.len() {
                return 0;
            } else {
                self.execute();
                let pc_history: Vec<usize> = self.history.iter().map(|i| i.pc).collect();
                if pc_history.contains( &self.state.pc ) {
                    return -1;
                }
                self.history.push( self.state );
            }
        }
    }

    fn execute(&mut self) -> usize {
        let i = self.rom[self.state.pc];
        match i {
            Instruction::Acc(arg) => self.acc(arg),
            Instruction::Jmp(arg) => self.jmp(arg),
            Instruction::Nop(_) => self.nop(),
        }

        self.state.pc
    }

    fn acc(&mut self, a: i64) {
        self.state.acc += a;
        self.state.pc += 1;
    }

    fn jmp(&mut self, a: i64) {
        self.state.pc = usize::try_from(i64::try_from(self.state.pc).unwrap() + a).unwrap();
    }

    fn nop(&mut self) {
        self.state.pc += 1;
    }
}

fn pt1( program: &Vec<Instruction> ) -> i64 {
    let mut comp = Computer::new( program.clone() );

    while comp.run() > 0 {}

    comp.state.acc
}

fn pt2( program: &Vec<Instruction> ) -> i64 {
    loop {
        for (i, instruction) in program.iter().enumerate() {
            let mut comp = Computer::new( program.clone() );
            match instruction {
                Instruction::Acc(_) => print!(""),
                Instruction::Jmp(arg) => comp.rom[i] = Instruction::Nop(*arg),
                Instruction::Nop(arg) => comp.rom[i] = Instruction::Jmp(*arg),
            }
            if comp.run() == 0 {
                return comp.state.acc;
            }
        }
    }
}

fn main() {
    let file = fs::read_to_string("input").expect("File not found");

    let program: Vec<Instruction> = file.trim().lines().map(|s| s.parse().unwrap()).collect();

    println!("Part 1: {}", pt1( &program ) );
    println!("Part 2: {}", pt2( &program ) );
}

