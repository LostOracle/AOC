use std::fmt;
use std::fs;
use std::env;

#[derive(Default)]
struct Entry {
    val: u64,
    hit: bool,
}

#[derive(Default)]
struct Board {
    data: Vec<Vec<Entry>>, // Row-major order
}
impl Board {
    fn new() -> Board {
        Default::default()
    }

    fn mark(&mut self, num: u64) {
        for row in self.data.iter_mut() {
            for col in row.iter_mut() {
                if col.val == num { col.hit = true }
            }
        }
    }
}
impl fmt::Display for Board {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        for row in self.data.iter() {
            for entry in row.iter() {
                write!(f, "{:>3}{}", entry.val, match entry.hit { true => 'X', false => '-' })?;
            }
            write!(f, "\n")?;
        }
        Ok(())
    }
}

// Wrapper so we can define a display for the board list
struct BoardList<'a>(&'a Vec<Board>);
impl fmt::Display for BoardList<'_> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        for entry in self.0.iter().enumerate() {
            write!(f, "{}:\n", entry.0)?;
            write!(f, "{}\n", entry.1)?;
        }
        Ok(())
    }
}

// Identifies if the board is a winner
// Returns true if board has a filled row / column,
// and the sum of un-marked entries.
fn winning_board(board: &Board) -> (bool,u64) {
    // Check rows
    let row_win = board.data.iter()
        .any(|row| {
            row.iter().all(|entry| entry.hit)
        });

    // Check columns
    let col_win = (0..board.data[0].len())
        .any(|col| {
            board.data.iter().all(|row| row[col].hit)
        });

    // Sum un-hit entries
    let sum: u64 = board.data.iter().fold(0, |sum: u64, row| {
        sum + row.iter()
            .filter(|entry| !entry.hit)
            .map(|entry| entry.val)
            .sum::<u64>()
    });
    return (row_win || col_win, sum);
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");

    let mut lines = input.lines();
    let number_sequence: Vec<u64> = lines.next().unwrap()
        .split(',')
        .map(|x| x.parse::<u64>().unwrap()).collect();

    lines.next(); // Toss a line

    let mut curr_board = Board::new();
    let mut board_list = Vec::<Board>::new();
    // Capture the set of bingo boards
    while let Some(line) = lines.next() {
        let mut row = Vec::<Entry>::new();
        line.split_whitespace().for_each(|x| {
            row.push(Entry{
                val: x.parse::<u64>().unwrap(),
                hit: false })
        });

        // Empty row means this is the end of the current board
        if row.len() == 0 {
            board_list.push(curr_board); 
            curr_board = Board::new();
        }
        else {
            curr_board.data.push(row);
        }
    }

    // Now execute the game
    // Satisfies both part 1 and part 2
    for num in number_sequence {
        for board in board_list.iter_mut() {
            // Mark down the called # if it exists
            board.mark(num);
        }

        let mut winners = Vec::<(usize, u64)>::new();

        // Identify winner(s)
        for board in board_list.iter().enumerate() {
            let win_info = winning_board(board.1); // Did this board win?
            let unmarked_sum = win_info.1;
            let board_id = board.0;

            if win_info.0 {
                winners.push((board_id, unmarked_sum));
            }
        }
        // Sort winners in REVERSE order so we remove from the higher end first
        winners.sort_by(|a, b| b.0.cmp(&a.0));
        
        // Identify and then filter out all winners
        if winners.len() > 0 {
            //println!("Boards: \n{}", BoardList(&board_list));

            for w in winners {
                println!("Found a winning board! ID({}), Score: {}*{}={}",
                    w.0, w.1, num, w.1 * num);

                // Remove the winning board - we don't care about it anymore
                board_list.remove(w.0);
            }
        }

        // Continue checking until all boards have won
        if board_list.len() == 0 { break; }
    }
}
