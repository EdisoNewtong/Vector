use blog::Post;

fn v1()
{
    println!("========== v1 Begin ==========\n");
    /*
    let mut post = Post::new();

    // 编写blog的内容
    post.add_text("I ate a salad for lunch today");
    assert_eq!("", post.content() );

    // 提交审阅
    post.request_review();
    assert_eq!("", post.content() );

    // 审阅通过了，发布这条blog
    post.approve();
    assert_eq!("I ate a salad for lunch today", post.content() );
    */
    println!("========== v1 End ==========");
}

fn v2()
{
    println!("========== v2 Begin ==========\n");
    let mut post = Post::new();  // post_1
    // 编写blog的内容
    post.add_text("I ate a salad for lunch today");

    // 提交审阅 
    // post_2   = post_1.request_review()
    // override post_1 defined @line24  by post_2
    let post = post.request_review();  // post_2

    // 审阅通过了，发布这条blog
    // post_3   = post_2.approve()
    // override post_2 defined @line24  by post_3
    let post = post.approve(); // post_3
    assert_eq!("I ate a salad for lunch today", post.content() );
    println!("========== v2 End ==========");
}


fn main() {
    v1();
    println!();
    v2();
}


