struct Aaa {
    width: i32,
    height: i32,
}

trait GotArea { fn area(&self) -> i32; }



impl GotArea for Aaa {
    fn area(&self) -> i32 {
        println!("GotArea ...");
        self.width * self.height
    }
}

impl Aaa {
    fn area(&self) -> i32 {
        println!("impl Aaa ...");
        self.width * self.height
    }
}


fn main() {
    let width = 5;
    let height = 3;

    let rect1: Aaa = Aaa {
        width, height,
    };

    let rect2: &dyn GotArea = &Aaa {
        width, height,
    };

    // call impl Aaa ...
    println!("Rect #1 : area = {}", rect1.area() );

    println!();

    // call GotArea ...
    println!("Rect #2 : area = {}", rect2.area() );

}



