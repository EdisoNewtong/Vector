
pub trait Draw {
    // the function "draw" is a public function by default 
    fn draw(&self);
}

/********************************************************************************
 
 The difference between #1 and #2 is that 
   in #2 the generic type T can be given only 1 time , 
  once it has been given , it has no chance to change itself to another type

e.g. : 

   Both Button and TextBox has implemented the trait Draw
   Screen<Button> can't change itself to Screen<TextBox> at run-time

  Alternatively, 
The defination #1 can adapt either Button or TextBox at any run-time


*********************************************************************************/
// defination #1 
pub struct Screen          { pub components: Vec< Box< dyn Draw> >, }
/********************************************************************************
// defination #2 
pub struct Screen<T: Draw> { pub components: Vec<T>, }
*********************************************************************************/

impl Screen {
    pub fn run(&self) {
        for component in self.components.iter() {
            component.draw();
        }
    }
}

// impl<T> Screen<T>
// where T: Draw,
// {
//     pub fn run(&self) {
//         for component in self.components.iter() {
//             component.draw();
//         }
//     }
// }

pub struct Button {
    pub width: u32,
    pub height: u32,
    pub label: String,
}

impl Draw for Button {
    fn draw(&self) {
        println!("Draw a [Button]");
    }
}


