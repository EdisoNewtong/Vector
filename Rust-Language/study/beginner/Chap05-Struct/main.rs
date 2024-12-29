
struct User {
    username: String, 
    sex:      bool,      // true for male , otherwise  for female
    age:      i32,
    email:    String,

}/*;*/



//
// Syntactic Sugar : 
//                   the parameter's name must be as same as the   field name inside the struct's defination 
//   
fn build_user(username: String, email: String) -> User {
    User {
        username,    // syntactic sugar : because there is a field whose name is  'username' , and its value will be asssigned as the Value of passed function argument 'username' 
        email,       // syntactic sugar : because there is a field whose name is  'email'    , and its value will be asssigned as the Value of passed function argument 'email'    
        sex: true,
        age: 15,
    }
}

fn create_struct_objects_sample_code()
{
    println!("---------- Start Generate struct objects code ----------");
    let user1: User = User {
        age:      12,
        username: String::from("Tom"),
        email: String::from("aabbcc@163.com"),
        sex:   true,
    };


    // apply     syntactic sugar #1 
    let user2: User = build_user( String::from("Mary"), String::from("Blue-Mary@gmail.com") );

    let username: String = String::from("Mary");
    let email:    String = String::from("Blue-Mary@gmail.com");

    // apply     syntactic sugar #2 ( the argument name must be as same as the field name inside the struct's defination )
    let user3: User = User {
        username,
        email,
        age: 44,
        sex: false,
    };

    // apply     syntactic sugar #3 
    let user4: User = User {
        username: String::from("Jack"),
        email: String::from("Jackwolfskin@jw.com"),

        /***************************************************
           user3 copy all of the field ( 'age' and 'sex' ) from user1 except the code specified fields :  'username' and 'email'
        ****************************************************/
        // Notes : 'user1' should be the last part of the initialization , and no comma after it
        .. user1   // Notes: there is a <Space> between '..' and user1         // or  ..user1 ( delete the <space> )
    };

    println!("user1 = {{\n\tname: {},\n\tage: {},\n\temail: {},\n\tsex: {}\n}}",  user1.username, user1.age, user1.email, (if user1.sex { "male" }   else  { "female" }  ) ); 
    println!("user2 = {{\n\tname: {},\n\tage: {},\n\temail: {},\n\tsex: {}\n}}",  user2.username, user2.age, user2.email, (if user2.sex { "male" }   else  { "female" }  ) ); 
    println!("user3 = {{\n\tname: {},\n\tage: {},\n\temail: {},\n\tsex: {}\n}}",  user3.username, user3.age, user3.email, (if user3.sex { "male" }   else  { "female" }  ) ); 
    println!("user4 = {{\n\tname: {},\n\tage: {},\n\temail: {},\n\tsex: {}\n}}",  user4.username, user4.age, user4.email, (if user4.sex { "male" }   else  { "female" }  ) ); 
}

fn tuple_struct_sample_code() 
{
    println!("---------- Start tuple struct objects code ----------");

    //  defination of color        
    //          (red,  green, blue)
    //          
    //  You'd better use :
    //
    //     C ( UpperCase
    //     ^
    struct Color(i32,   i32,    i32);
    let red_color:   Color = Color(255,0,0);
    let green_color: Color = Color(0,255,0);

    println!("red_color.value   = ({}, {}, {})", red_color.0,   red_color.1,   red_color.2 );
    println!("green_color.value = ({}, {}, {})", green_color.0, green_color.1, green_color.2 );

    //  defination of coordinates in a 3D world        
    //                    (x,   y,   z)
    struct Coordinates3d(f32, f32, f32);
    let position1: Coordinates3d  = Coordinates3d(1.0,  0.0,  0.0);
    let position2: Coordinates3d  = Coordinates3d(-2.3, 4.5, -9.7);

    println!("pos1.value   = ({}, {}, {})",  position1.0,   position1.1,   position1.2 );
    println!("pos2.value   = ({}, {}, {})",  position2.0,   position2.1,   position2.2 );
}


fn unit_like_struct()
{
    println!("---------- Start unit like struct code ----------");
    // empty field struct defination is valid  
    //                                         for  trait(interface) feature
    struct EmptyStruct {
    }

    // #[warn(unused_variables)]
    let _obj1: EmptyStruct = EmptyStruct{};
}

/**
    the following line     
#[derive(Debug)]    
                     is  a  <must>

Otherwise !!! Compiler Error !!!
    because println!( object of a struct Rectangle ) need this " directives "

*/
#[derive(Debug)]
struct Rectangle {
    width:  i32,
    height: i32,
}

impl Rectangle {
    fn area(&self) -> i32 {
        self.width * self.height
    }

    // static function
    fn create_a_rect(width: i32, height: i32) -> Rectangle {
        Rectangle {
            width,
            height
        }
    }
}

fn rectangle_struct_sample_code() {
    println!("---------- Start Rectangle struct code ----------");
    //                 Rectangle static function
    let r: Rectangle = Rectangle::create_a_rect(2,4);
    let s: i32 = r.area();
    println!("r.area = {}", s);
    println!("--------------------------------------------------");
    println!("Normal rect = {:?}", r);
    println!("--------------------------------------------------");
    println!("Pretty rect = {:#?}", r);
    println!("--------------------------------------------------");
}

fn main() {
    create_struct_objects_sample_code();
    println!();
    println!();
    tuple_struct_sample_code();
    println!();
    println!();
    unit_like_struct();

    println!();
    println!();
    rectangle_struct_sample_code();

}

