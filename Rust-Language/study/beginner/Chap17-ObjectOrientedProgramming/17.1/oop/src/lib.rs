// Notes : 
// struct AveragedCollection is public
//    the two members "list and average" are not available to the caller
//
pub struct AveragedCollection {
    list: Vec<i32>,
    average: f64,
}

impl AveragedCollection {
    /**************************************************
         4 public interface for the caller
      1. new()
      2. add(...)
      3. remove(...)
      4. average(...)

    ***************************************************/

    // 1.
    pub fn new() -> AveragedCollection {
        AveragedCollection {
            list: vec![],
            average: 0.0 as f64,
        }
    }

    // 2.
    pub fn add(&mut self, value: i32) {
        self.list.push(value);
        // call internal private function to update the average
        self.update_average();
    }

    // 3.
    pub fn remove(&mut self) -> Option<i32> {
        let result = self.list.pop();
        match result {
            Some(value) => {
                // call internal private function to update the average
                self.update_average();
                // return the removed value
                Some(value)
            },
            None => None,
        }
    }


    // 4.
    // the average number can be get outside
    pub fn average(&self) -> f64 {
        self.average
    }

    /*private*/ 
    fn update_average(&mut self) {
        let total: i32 = self.list.iter().sum();
        self.average = total as f64 / self.list.len() as f64;
    }
}




