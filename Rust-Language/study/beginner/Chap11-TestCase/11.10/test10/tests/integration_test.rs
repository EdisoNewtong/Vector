use test10;

mod util;

#[test]
fn it_adds_2() {
    util::common_helper_util();
    assert_eq!(5, test10::add_2(3) )
}


#[test]
fn it_adds_3() {
    util::common_helper_util();
    assert_eq!(10, test10::add_3(7) )
}


