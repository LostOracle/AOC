use std::fs;
use std::fmt;
use std::env;

struct Packet {
    id: usize,
    parent: usize,
    version: usize,
    type_id: usize,
    len: usize,
    val: u64
}

struct PktParser {
    id: usize
}

impl PktParser {
    fn new_pkt(&mut self) -> Packet {
        self.id += 1;

        Packet {
            id: self.id,
            parent: 0,
            version: 0,
            type_id: 0,
            len: 0,
            val: 0
        }
    }

    fn parse_operator(&mut self, bin_str: &str, header_len: &mut usize, parent_id: usize) -> Vec<Packet> {
        println!("Parsing operator packet");
    
        let mut version_sum = 0;
        let mut all_pkts = Vec::<Packet>::new();
    
        // Get length type ID
        let length_type = u8::from_str_radix(&bin_str[0..1], 2).unwrap();
    
        match length_type {
            0 => {
                const LEN_BITS: usize = 15;
                let mut pkt_start = 1+LEN_BITS;
                *header_len += pkt_start;
    
                let total_len = usize::from_str_radix(&bin_str[1..pkt_start], 2).unwrap();
                println!("Sub-packet len: {} = {}", &bin_str[1..pkt_start], total_len);
    
                while pkt_start < 1+LEN_BITS+total_len {
                    let pkts = self.parse(&bin_str[pkt_start..], parent_id);
                    let pkt_len = pkts.iter().fold(0, |sum,p| sum + p.len);
                    pkt_start += pkt_len;
    
                    all_pkts.extend(pkts);
                }
                println!("Done sub-packet len");
            },
            _ => {
                const LEN_PKT_CT: usize = 11;
                let mut pkt_start = 1+LEN_PKT_CT;
                *header_len += pkt_start;
    
                let num_sub_packets = usize::from_str_radix(&bin_str[1..pkt_start], 2).unwrap();
            println!("Num sub-packets: {}", num_sub_packets);

            for pkt in 0..num_sub_packets {
                let pkts = self.parse(&bin_str[pkt_start..], parent_id);
                let pkt_len = pkts.iter().fold(0, |sum,p| sum + p.len);
                pkt_start += pkt_len;

                all_pkts.extend(pkts);
            }
            println!("Done sub-packets");
        },
    }

    all_pkts
}

// Calculates version numbers in all packets
fn parse(&mut self, bin_str: &str, parent_id: usize) -> Vec<Packet> {
    let mut pkts = Vec::<Packet>::new();

    // Get version and type ID
    let version = usize::from_str_radix(&bin_str[0..3], 2).unwrap();
    let type_id = usize::from_str_radix(&bin_str[3..6], 2).unwrap();
    let mut len = 0;
    let mut val: u64 = 0;

    let mut pkt = self.new_pkt();

    println!("Parsing packet: v={}, t={}", version, type_id);
    // Handle packet type
    match type_id {
        4 => { 
            println!("Literal");
            // Literal packet - parse literal
            const CHUNK_LEN: usize = 5;
            const VAL_START: usize = 6; // value start begins after 3 header fields
            let mut vs = VAL_START;
            let mut keep_parsing = 0;
            let mut data = String::new();
            loop {
                data.extend(bin_str[vs+1..vs+CHUNK_LEN].chars());
                keep_parsing = usize::from_str_radix(&bin_str[vs..vs+1], 2).unwrap();
                vs += CHUNK_LEN;
                if keep_parsing == 0 { break; }
            }
            val = u64::from_str_radix(&data, 2).unwrap();

            //let missing = (vs - VAL_START) % 4;
            //let pad_len = if missing != 0 { 4 - missing } else { 0 };
            len = vs;
        },
        _ => {
            let mut header_len = 6;
            let new_pkts = self.parse_operator(&bin_str[6..], &mut header_len, pkt.id);
            match type_id {
                0 => { 
                    val = new_pkts.iter()
                        .filter(|x| x.parent == pkt.id)
                        .fold(0, |sum,x| sum + x.val);
                    println!("Sum packet = {}", val);
                },
                1 => { 
                    val = new_pkts.iter()
                        .filter(|x| x.parent == pkt.id)
                        .fold(1, |sum,x| sum * x.val);
                    println!("Prod packet = {}", val);
                },
                2 => { 
                    val = new_pkts.iter()
                        .filter(|x| x.parent == pkt.id)
                        .map(|x| x.val).min().unwrap();
                    println!("Min packet = {}", val);
                },
                3 => { 
                    val = new_pkts.iter()
                        .filter(|x| x.parent == pkt.id)
                        .map(|x| x.val).max().unwrap();
                    println!("Max packet = {}", val);
                },
                5 => { 
                    let mut pkt_iter = new_pkts.iter().filter(|x| x.parent == pkt.id);
                    let first = pkt_iter.next().unwrap();
                    let second = pkt_iter.next().unwrap();
                    val = if first.val > second.val { 1 } else { 0 };
                    println!("Greater-than packet {} > {} ? {}", first.val, second.val, val);
                },
                6 => { 
                    let mut pkt_iter = new_pkts.iter().filter(|x| x.parent == pkt.id);
                    let first = pkt_iter.next().unwrap();
                    let second = pkt_iter.next().unwrap();
                    val = if first.val < second.val { 1 } else { 0 };
                    println!("Less-than packet {} < {} ? {}", first.val, second.val, val);
                },
                7 => { 
                    let mut pkt_iter = new_pkts.iter().filter(|x| x.parent == pkt.id);
                    let first = pkt_iter.next().unwrap();
                    let second = pkt_iter.next().unwrap();
                    val = if first.val == second.val { 1 } else { 0 };
                    println!("Equal-to packet {} == {} ? {}", first.val, second.val, val);
                },
                _ => { panic!("Unwanted type id {}", type_id)},
            }
            len = header_len; // Don't include sub-packet length
            pkts.extend(new_pkts);
        }
        
    }

    pkt.parent = parent_id;
    pkt.version = version;
    pkt.type_id = type_id;
    pkt.len = len;
    pkt.val = val;
    pkts.push(pkt);
    println!("V = {}, T = {}, val = {}, len = {}", version, type_id, val, len);
    println!("Pkt data = {}", &bin_str[0..len]);


    pkts
}

}

fn to_bin_str(hex_str: &str) -> String {
    hex_str
        .chars()
        .map(|c| u8::from_str_radix(&c.to_string(), 16).unwrap()) // Convert from hex to number
        .fold(String::new(), |sum,x| sum + format!("{:>04b}", x).as_str()) // Convert to binary string, 0-padded
}

fn version_sum(pkts: &Vec<Packet>) -> usize {
    pkts.iter().fold(0, |sum,p| sum + p.version)
}

fn main() {
    let in_file = env::args().collect::<Vec<String>>();
    let in_file = in_file.iter().last()
        .expect("Needs input file name");

    let input = fs::read_to_string(in_file)
        .expect("Unable to open input file");

    let bin_str = to_bin_str(input.lines().next().unwrap());
    println!("Bin String: {}", bin_str);

    // Parse the packet tree
    let mut pkt_parser = PktParser{id: 0};
    let pkts = pkt_parser.parse(&bin_str, 0);
    println!("Version sum = {}", version_sum(&pkts));

    // Part 2 - calculate value
    println!("Value = {}", pkts.last().unwrap().val);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_a() {
        let bin_str = to_bin_str("8A004A801A8002F478");
        let mut pkt_parser = PktParser{id: 0};
        let pkts = pkt_parser.parse(&bin_str, 0);
        assert_eq!(version_sum(&pkts), 16);
    }

    #[test]
    fn test_b() {
        let bin_str = to_bin_str("620080001611562C8802118E34");
        let mut pkt_parser = PktParser{id: 0};
        let pkts = pkt_parser.parse(&bin_str, 0);
        assert_eq!(version_sum(&pkts), 12);
    }

    #[test]
    fn test_c() {
        let bin_str = to_bin_str("C0015000016115A2E0802F182340");
        let mut pkt_parser = PktParser{id: 0};
        let pkts = pkt_parser.parse(&bin_str, 0);
        assert_eq!(version_sum(&pkts), 23);
    }

    #[test]
    fn test_d() {
        let bin_str = to_bin_str("A0016C880162017C3686B18A3D4780");
        let mut pkt_parser = PktParser{id: 0};
        let pkts = pkt_parser.parse(&bin_str, 0);
        assert_eq!(version_sum(&pkts), 31);
    }

    #[test]
    fn test_e() {
        let bin_str = to_bin_str("EE00D40C823060");
        let mut pkt_parser = PktParser{id: 0};
        let pkts = pkt_parser.parse(&bin_str, 0);
        assert_eq!(version_sum(&pkts), 14);
    }

    #[test]
    fn test_f() {
        let bin_str = to_bin_str("38006F45291200");
        let mut pkt_parser = PktParser{id: 0};
        let pkts = pkt_parser.parse(&bin_str, 0);
        assert_eq!(version_sum(&pkts), 9);
    }

    #[test]
    fn test_sum_pkt() {
        let bin_str = to_bin_str("C200B40A82");
        let mut pkt_parser = PktParser{id: 0};
        let pkts = pkt_parser.parse(&bin_str, 0);
        assert_eq!(pkts.last().unwrap().val, 3);
    }

    #[test]
    fn test_prod_pkt() {
        let bin_str = to_bin_str("04005AC33890");
        let mut pkt_parser = PktParser{id: 0};
        let pkts = pkt_parser.parse(&bin_str, 0);
        assert_eq!(pkts.last().unwrap().val, 54);
    }

    #[test]
    fn test_min_pkt() {
        let bin_str = to_bin_str("880086C3E88112");
        let mut pkt_parser = PktParser{id: 0};
        let pkts = pkt_parser.parse(&bin_str, 0);
        assert_eq!(pkts.last().unwrap().val, 7);
    }

    #[test]
    fn test_max_pkt() {
        let bin_str = to_bin_str("CE00C43D881120");
        let mut pkt_parser = PktParser{id: 0};
        let pkts = pkt_parser.parse(&bin_str, 0);
        assert_eq!(pkts.last().unwrap().val, 9);
    }

    #[test]
    fn test_less_than() {
        let bin_str = to_bin_str("D8005AC2A8F0");
        let mut pkt_parser = PktParser{id: 0};
        let pkts = pkt_parser.parse(&bin_str, 0);
        assert_eq!(pkts.last().unwrap().val, 1);
    }

    #[test]
    fn test_greater_than() {
        let bin_str = to_bin_str("F600BC2D8F");
        let mut pkt_parser = PktParser{id: 0};
        let pkts = pkt_parser.parse(&bin_str, 0);
        assert_eq!(pkts.last().unwrap().val, 0);
    }

    #[test]
    fn test_equal_to() {
        let bin_str = to_bin_str("9C005AC2F8F0");
        let mut pkt_parser = PktParser{id: 0};
        let pkts = pkt_parser.parse(&bin_str, 0);
        assert_eq!(pkts.last().unwrap().val, 0);
    }

    #[test]
    fn test_complex() {
        let bin_str = to_bin_str("9C0141080250320F1802104A08");
        let mut pkt_parser = PktParser{id: 0};
        let pkts = pkt_parser.parse(&bin_str, 0);
        assert_eq!(pkts.last().unwrap().val, 1);
    }
}
