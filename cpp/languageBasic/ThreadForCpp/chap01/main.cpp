#include <iostream>
#include <thread>
using namespace std;

void helloThread()
{
	cout << "Hello Thread " << endl;
}




int main( int argc , char* argv[], char* env[])
{
	thread t(helloThread);
	// t.join(); is the core sentence in this program
	t.join();



	cout << "Hello World" << endl;
	return 0;
}
