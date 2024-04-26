

fn t1()
{
    let num: i32 = 15;
    let ref_num: &i32 = &num;
    let pointer_num: *const i32 = &num;

    println!("a = {}, refNum = {} ", num, ref_num);
    unsafe {
        println!("pointer_num = {:p}, *pointer_num = {} ", pointer_num, *pointer_num);
    }
}

fn main()
{
    // println!("Hello Rust");
    t1();
}

