use std::fmt;

#[derive(Debug)]
struct Test
{
    a: f32,
    b: i8
}

impl fmt::Display for Test
{
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result
    {
        write!(f, "{} {}", self.a, self.b )
    }
}

fn main()
{
    let t = Test { a:0.0, b:0 };

    println!("{}", t );
}
