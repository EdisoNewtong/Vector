
mod m_a;
mod m_b;

fn main()
{
    let a: i32 = 70;
    let b: i32 =  3;
    let ret1: i32 = m_a::my_impl::my_add(a,b); // return a * b  => 70 * 3 = 210
    let ret2: i32 = m_b::my_impl::my_add(a,b); // return a % b  => 70 % 3 =   1
	println!("ret1 = {a} * {b} = {ret1}");
	println!("ret2 = {a} % {b} = {ret2}");


}


