#include <iostream>
#include <vector>
using namespace std;

class Operator {
public:
    virtual ~Operator() { }
    virtual int calculate(int a,int b) = 0;
};

class Add : public Operator {
public:
    virtual int calculate(int a,int b) override { return a + b; }
};

class Minus : public Operator {
public:
    virtual int calculate(int a,int b) override { return a - b; }
};

class Multiply : public Operator {
public:
    virtual int calculate(int a,int b) override { return a * b; }
};

class Devide : public Operator {
public:
    virtual int calculate(int a,int b) override { return a / b; }
};

class Mod : public Operator {
public:
    virtual int calculate(int a,int b) override { return a % b; }
};

class OperatorFactory {
public:
    static Operator* createOperatorByName(char type)
    {
        Operator* generated_op = nullptr;
        switch( type )
        {
        case '+':
            generated_op = new Add();
            break;
        case '-':
            generated_op = new Minus();
            break;
        case '*':
            generated_op = new Multiply();
            break;
        case '/':
            generated_op = new Devide();
            break;
        case '%':
            generated_op = new Mod();
            break;
        default:
            cout << "[Error] : Unknown type of " << type << endl;
            break;
        }

        // push it to memory pool if valid
        if ( generated_op != nullptr ) {
            s_opPool.push_back( generated_op );
        }

        return generated_op;
    }

    static void releaseOperatorPool()
    {
        for( auto& e : s_opPool ) {
            if ( e != nullptr ) { delete e; e = nullptr; }
        }
        s_opPool.clear();
    }
protected:
    static vector<Operator*> s_opPool;
};

// static
vector<Operator*> OperatorFactory::s_opPool{ };


int main(int argc, char* argv[], char* env[])
{
	(void)env;

    int a = 14;
    int b = 3;
    int opRet = 0;

    auto addOp = OperatorFactory::createOperatorByName('+');
    if( addOp != nullptr ) { opRet = addOp->calculate(a,b); cout << a << " + " << b << " = " << opRet << endl; }

    auto minusOp = OperatorFactory::createOperatorByName('-');
    if( minusOp != nullptr ) { opRet = minusOp->calculate(a,b); cout << a << " - " << b << " = " << opRet << endl; }

    auto multiplyOp = OperatorFactory::createOperatorByName('*');
    if( multiplyOp != nullptr ) { opRet = multiplyOp->calculate(a,b); cout << a << " * " << b << " = " << opRet << endl; }

    auto devideOp = OperatorFactory::createOperatorByName('/');
    if( devideOp != nullptr ) { opRet = devideOp->calculate(a,b); cout << a << " / " << b << " = " << opRet << endl; }

    auto modOp = OperatorFactory::createOperatorByName('%');
    if( modOp != nullptr ) { opRet = modOp->calculate(a,b); cout << a << " % " << b << " = " << opRet << endl; }

    auto lessThanOp = OperatorFactory::createOperatorByName('<');
    if( lessThanOp != nullptr ) { opRet = lessThanOp->calculate(a,b); cout << a << " < " << b << " = " << opRet << endl; }


    OperatorFactory::releaseOperatorPool();
    

	return 0;
}


