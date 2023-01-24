#include <iostream>
#include <string>

// #include <format>
#include <fmt/format-inl.h>
using namespace std;



void test1()
{
    string title{"Hello : MX Linux "};
    constexpr int VERSION = 21;
    title += to_string(VERSION);
    cout << title << endl;

    int age = 32;
    string strAge = "age";

    // auto formated_strError = std::format("{} = {}", strAge, age);

    // formated_str is a 'std::string'   type
    auto formated_str = fmt::format("{} = {}", strAge, age);
    cout << formated_str << endl;

}


int main(int argc, char* argv[], char* env[])
{
    test1();
    return 0;
}
