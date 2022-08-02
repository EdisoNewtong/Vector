//
// as the tutorial show :
//
//    You'd better write such statement at the 1st line of this such in order to declare it as a named exported   module
export module aaaYourhello;


export int hello_DoModule(int a,int b)
{
	return a % b;
}


// without  keyword "export"  , internal use only
// export 
const char* hello2()
{
	return "Hello World internal !";
}

