/*  Inline, within curly brackets that replace the semicolon following mod garden
mod garden;   */

mod garden {
    pub fn water_flowers(ml: f64) {
        println!("-----------------------------------------------");
        println!("I waters flowers with {} ml every days.", ml);
        println!("-----------------------------------------------");
    }
}



fn main() {
    garden::water_flowers(1.2);
}

