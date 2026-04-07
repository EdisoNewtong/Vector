use oop::AveragedCollection;

fn test_1()
{
    let mut ac: AveragedCollection = AveragedCollection::new();

    ac.add(10);
    println!("1st. push 10, avg = {}", ac.average() );
    ac.add(20);
    println!("2nd. push 20, avg = {}", ac.average() );
    ac.add(30);
    println!("3rd. push 30, avg = {}", ac.average() );

}


fn main() {
    test_1();
}



