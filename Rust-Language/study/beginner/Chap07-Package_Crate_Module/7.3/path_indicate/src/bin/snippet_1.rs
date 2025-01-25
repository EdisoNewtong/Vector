////////////////////////////////////////////////////
//
//     src : snippet_1.rs
//
////////////////////////////////////////////////////
fn server_order() 
{
    println!("in root #1 : function server_order()");
}



mod back_of_house {
    fn fix_incorrect_order() 
    {
        cook_order();

        // 2 ways to call   function 'server_order'  defined @line:2
        // use relative path
        super::server_order();
        // use absolute path
        crate::server_order();
    }

    fn cook_order() 
    {
        println!("in root   function server_order()");
    }


}



fn main()
{
    server_order();
}


