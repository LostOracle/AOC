use std::fs;
use std::fmt;
use std::env;

struct Packet {
    version: usize,
    type_id: usize,
    len: usize,
    val: u64
}

fn parse_operator(bin_str: &str, header_len: &mut usize) -> Vec<Packet> {
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
                let pkts = parse(&bin_str[pkt_start..]);
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
                let pkts = parse(&bin_str[pkt_start..]);
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
fn parse(bin_str: &str) -> Vec<Packet> {
    let mut pkts = Vec::<Packet>::new();

    // Get version and type ID
    let version = usize::from_str_radix(&bin_str[0..3], 2).unwrap();
    let type_id = usize::from_str_radix(&bin_str[3..6], 2).unwrap();
    let mut len = 0;
    let mut val: u64 = 0;

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
            // Operator packet
            let mut header_len = 6;
            let new_pkts = parse_operator(&bin_str[6..], &mut header_len);
            //len = new_pkts.iter().fold(0, |sum,p| sum + p.len) + header_len;
            len = header_len; // Don't include sub-packet length
            pkts.extend(new_pkts);
        },
    }

    pkts.push(Packet{ version: version, type_id: type_id, len: len, val: val });
    println!("V = {}, T = {}, val = {}, len = {}", version, type_id, val, len);
    println!("Pkt data = {}", &bin_str[0..len]);


    pkts
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
    let pkts = parse(&bin_str);
    println!("Version sum = {}", version_sum(&pkts));
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_a() {
        let bin_str = to_bin_str("8A004A801A8002F478");
        let pkts = parse(&bin_str);
        assert_eq!(version_sum(&pkts), 16);
    }

    #[test]
    fn test_b() {
        let bin_str = to_bin_str("620080001611562C8802118E34");
        let pkts = parse(&bin_str);
        assert_eq!(version_sum(&pkts), 12);
    }

    #[test]
    fn test_c() {
        let bin_str = to_bin_str("C0015000016115A2E0802F182340");
        let pkts = parse(&bin_str);
        assert_eq!(version_sum(&pkts), 23);
    }

    #[test]
    fn test_d() {
        let bin_str = to_bin_str("A0016C880162017C3686B18A3D4780");
        let pkts = parse(&bin_str);
        assert_eq!(version_sum(&pkts), 31);
    }

    #[test]
    fn test_e() {
        let bin_str = to_bin_str("EE00D40C823060");
        let pkts = parse(&bin_str);
        assert_eq!(version_sum(&pkts), 14);
    }

    #[test]
    fn test_f() {
        let bin_str = to_bin_str("38006F45291200");
        let pkts = parse(&bin_str);
        assert_eq!(version_sum(&pkts), 9);
    }
}
