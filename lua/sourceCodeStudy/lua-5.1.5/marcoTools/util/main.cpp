#include <iostream>
#include <string>
using namespace std;

static int printFlag = 0;

string remove2quote(const string& withquoto);

string MYTMP(const string& arg1)
{

	string sComment  = 
R"(
/*
    ....
*/
)";
	string sTemplate =
R"(
	...
)";
	
	const string S1 = "__ARG__1";

	string::size_type pos = 0;
	while ( (pos = sTemplate.find(S1,pos)) != string::npos ) {
		string replaceto = remove2quote(arg1);
		sTemplate.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}

	if ( printFlag ) {
		cout << "==================================================" << endl;
		cout << sComment + sTemplate << endl;
		cout << "==================================================" << endl;
	}

	return sTemplate;

}


string remove2quote(const string& withquoto)
{
	string retstr = withquoto;
	char quote = '"';
	if ( withquoto.size() >= 2) {
		if ( withquoto.at(0) == quote && withquoto.at( withquoto.size()-1) == quote ) {
			retstr = withquoto.substr(1, withquoto.size() - 2 );
		}
	} 

	return retstr;
}

string GET_OPCODE(const string& arg)
{
/*


#define POS_OP		0
#define SIZE_OP		6

#define GET_OPCODE(i)	(cast(OpCode, ((i)>>POS_OP) & MASK1(SIZE_OP,0)))

#define GET_OPCODE(i)	(cast(OpCode, ((i)>>0) & MASK1(6,0)))




// #define MASK1(n,p)	((~((~(Instruction)0)<<n))<<p)

MASK1(6,0)	((~((~(Instruction)0)<<6))<<0)




typedef lu_int32 Instruction;

(OpCode)( ((__ARG__1)>>0)  &  ()   )

             GET_OPCODE(*previous)                                                     
      if ((((OpCode)(((*previous)>>0) & ((~((~(Instruction)0)<<6))<<0)))) == OP_LOADNIL)

*/


	string sTemplate =
R"(
/*
      typedef lu_int32 Instruction;

      lu_int32 left_Operand  = (__ARG__1) >> 0;
      lu_int32 right_Operand = ((~((~(Instruction)0)<<6))<<0);
	  (OpCode)( left_Operand &  right_Operand )
*/
)";
	  

	string sTemplate_2 = R"( ( (OpCode)( ( (__ARG__1) >> 0 )   & ((~((~(Instruction)0)<<6))<<0) ) ) )";

	const string S1 = "__ARG__1";

	string::size_type pos = 0;
	while ( (pos = sTemplate.find(S1,pos)) != string::npos ) {
		string replaceto = remove2quote(arg);
		sTemplate.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}

	pos = 0;
	while ( (pos = sTemplate_2.find(S1,pos)) != string::npos ) {
		string replaceto = remove2quote(arg);
		sTemplate_2.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}

	cout << "==================================================" << endl;
	cout << sTemplate + sTemplate_2 << endl;
	cout << "==================================================" << endl;

	return sTemplate_2;
}


string GETARG_A(const string& arg)
{
	string sComment =
R"(
/*

       #define POS_OP		0
       #define SIZE_OP		6

       #define POS_A		(POS_OP + SIZE_OP)  // 6

       #define SIZE_A		8
	   
       #define MASK1(n,p)	( (~((~(Instruction)0)<<n))<<p )

	   #define GETARG_A(i)	(cast(int, ((i)>>POS_A) & MASK1(SIZE_A,0)))
*/
)";

	string sTemplate = R"( (int)(  ( (__ARG__1) >> 6)   &  (  (~((~(Instruction)0)<<8))<<0  ) ) )";
	const string S1 = "__ARG__1";


	string::size_type pos = 0;
	while ( (pos = sTemplate.find(S1,pos)) != string::npos ) {
		string replaceto = remove2quote(arg);
		sTemplate.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}

	if ( printFlag ) {
		cout << "==================================================" << endl;
		cout << (sComment + sTemplate) << endl;
		cout << "==================================================" << endl;
	}

	return sTemplate;
}


string GETARG_B(const string& arg)
{
	string sComment =
R"(
/*
	#define GETARG_B(i)	(cast(int, ((i)>>POS_B) & MASK1(SIZE_B,0)))

	#define SIZE_B		9

	#define SIZE_C		9

	#define POS_C		(POS_A + SIZE_A)   // (6 + 8) = (14)
	#define POS_B		(POS_C + SIZE_C)   // (14+9)  = 23

*/)";


	string sTemplate = R"aaa( (int)(  ( (__ARG__1) >> 23)   & (  (~((~(Instruction)0)<< 9 ))<<0  ) ) )aaa";
	const string S1 = "__ARG__1";

	string::size_type pos = 0;
	while ( (pos = sTemplate.find(S1,pos)) != string::npos ) {
		string replaceto = remove2quote(arg);
		sTemplate.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}

	if ( printFlag ) {
		cout << "==================================================" << endl;
		cout << sTemplate << endl;
		cout << "==================================================" << endl;
	}
	return sTemplate;
}


string SETARG_B(const string& arg1, const string& arg2)
{
	string sComment =
R"(
/*
	#define GETARG_B(i)	(cast(int, ((i)>>POS_B) & MASK1(SIZE_B,0)))

	#define SIZE_B		9

	#define SIZE_C		9

	#define POS_C		(POS_A + SIZE_A)   // (6 + 8) = (14)
	#define POS_B		(POS_C + SIZE_C)   // (14+9)  = 23

	(int)(  ( (__ARG__1) >> 23)   & (  (~((~(Instruction)0)<< 9 ))<<0  ) )

	#define SETARG_B(i,b)	((i) = (((i)&MASK0(SIZE_B,POS_B)) | \
		((cast(Instruction, b)<<POS_B)&MASK1(SIZE_B,POS_B))))

	#define MASK0(n,p)	(~MASK1(n,p))

    #define MASK1(n,p)	((~((~(Instruction)0)<<n))<<p)
*/)";

	string sTemplate =
R"(
	     (  (__ARG__1) = 
		  ( (__ARG__1)             &  ( ~((~((~(Instruction)0)<<9))<<23) ) )
	     |  ( ( ((Instruction)(__ARG__2)) << 23 )   &  (  ((~((~(Instruction)0)<<9))<<23) ) ) );
)";

	const string S1 = "__ARG__1";
	const string S2 = "__ARG__2";

	string::size_type pos = 0;
	while ( (pos = sTemplate.find(S1,pos)) != string::npos ) {
		string replaceto = remove2quote(arg1);
		sTemplate.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}


	pos = 0;
	while ( (pos = sTemplate.find(S2,pos)) != string::npos ) {
		string replaceto = remove2quote(arg2);
		sTemplate.replace( pos, S2.size(), replaceto);
		pos += replaceto.size();
	}

	if ( printFlag ) {
		cout << "==================================================" << endl;
		cout << (sComment + sTemplate) << endl;
		cout << "==================================================" << endl;
	}

	return sTemplate;
}

void luaK_codeAsBx(const string& arg1, const string& arg2, const string& arg3, const string& arg4)
{

	string sTemplate =
R"(
/*
	#define luaK_codeAsBx(fs,o,A,sBx)	luaK_codeABx(fs,o,A,(sBx)+MAXARG_sBx)


	#define SIZE_C		9
	#define SIZE_B		9
	#define SIZE_Bx		(SIZE_C + SIZE_B) // 9 + 9 = 18

    #define MAXARG_Bx        ((1<<SIZE_Bx)-1)

    #define MAXARG_sBx        (MAXARG_Bx>>1)         //   ( ( (1<<18) -1 ) >> 1 )

	luaK_codeABx( __ARG__1, __ARG__2, __ARG__3, (__ARG__4) + ( ( (1<<18) -1 ) >> 1 ) );

*/
)";

	const string S1 = "__ARG__1";
	const string S2 = "__ARG__2";
	const string S3 = "__ARG__3";
	const string S4 = "__ARG__4";

	string::size_type pos = 0;
	while ( (pos = sTemplate.find(S1,pos)) != string::npos ) {
		string replaceto = remove2quote(arg1);
		sTemplate.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}


	pos = 0;
	while ( (pos = sTemplate.find(S2,pos)) != string::npos ) {
		string replaceto = remove2quote(arg2);
		sTemplate.replace( pos, S2.size(), replaceto);
		pos += replaceto.size();
	}

	pos = 0;
	while ( (pos = sTemplate.find(S3,pos)) != string::npos ) {
		string replaceto = remove2quote(arg3);
		sTemplate.replace( pos, S3.size(), replaceto);
		pos += replaceto.size();
	}

	pos = 0;
	while ( (pos = sTemplate.find(S4,pos)) != string::npos ) {
		string replaceto = remove2quote(arg4);
		sTemplate.replace( pos, S4.size(), replaceto);
		pos += replaceto.size();
	}

	cout << "==================================================" << endl;
	cout << sTemplate << endl;
	cout << "==================================================" << endl;
}


void SETARG_sBx(const string& arg1, const string& arg2)
{
	
	string sTemplate =
R"(
/*

    #define SETARG_Bx(i,b)	((i) = (((i)&MASK0(SIZE_Bx,POS_Bx)) | \
		    ((cast(Instruction, b)<<POS_Bx)&MASK1(SIZE_Bx,POS_Bx))))


	#define SIZE_A		8

	#define SIZE_OP		6

	#define POS_OP		0
	#define POS_A		(POS_OP + SIZE_OP) // 0 + 6 = 6
	#define POS_C		(POS_A + SIZE_A)   // 6 + 8 = 14
	#define POS_Bx		POS_C              // 14

	#define SETARG_sBx(i,b)	SETARG_Bx((i),cast(unsigned int, (b)+MAXARG_sBx))

	(__ARG__1) = (     ((__ARG__1)                                                                            & (  ~((~((~(Instruction)0)<<18))<<14) )  ) 
	                 | (( ( (Instruction)( (unsigned int)( (__ARG__2) + ( ( (1<<18) -1 ) >> 1 )) ) ) << 14 )  & (   ((~((~(Instruction)0)<<18))<<14) )  )
				   );
*/
)";

	const string S1 = "__ARG__1";
	const string S2 = "__ARG__2";

	string::size_type pos = 0;
	while ( (pos = sTemplate.find(S1,pos)) != string::npos ) {
		string replaceto = remove2quote(arg1);
		sTemplate.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}

	pos = 0;
	while ( (pos = sTemplate.find(S2,pos)) != string::npos ) {
		string replaceto = remove2quote(arg2);
		sTemplate.replace( pos, S2.size(), replaceto);
		pos += replaceto.size();
	}

	cout << "==================================================" << endl;
	cout << sTemplate << endl;
	cout << "==================================================" << endl;

}

void GETARG_sBx(const string& arg1)
{

	string sTemplate =
R"(
/*
	#define SIZE_Bx		(SIZE_C + SIZE_B) // 9 + 9 = 18

	#define POS_Bx		POS_C              // 14

    #define MAXARG_sBx        (MAXARG_Bx>>1)         //   ( ( (1<<18) -1 ) >> 1 )

	#define GETARG_Bx(i)	(cast(int, ((i)>>POS_Bx) & MASK1(SIZE_Bx,0)))

	#define GETARG_sBx(i)	(GETARG_Bx(i)-MAXARG_sBx)

	(int)( ( (__ARG__1) >> 14 ) & ((~((~(Instruction)0)<<18))<<0) )  -  ( ( (1<<18) -1 ) >> 1 );

*/
)";

	const string S1 = "__ARG__1";

	string::size_type pos = 0;
	while ( (pos = sTemplate.find(S1,pos)) != string::npos ) {
		string replaceto = remove2quote(arg1);
		sTemplate.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}

	cout << "==================================================" << endl;
	cout << sTemplate << endl;
	cout << "==================================================" << endl;

}


string testTMode(const string& arg1)
{
	string sTemplate =
R"(
/*
	#define testTMode(m)    (luaP_opmodes[m] & (1 << 7))

	(   luaP_opmodes[__ARG__1] 
	  & (1<<7) 
	)
*/
)";
	

	const string S1 = "__ARG__1";

	string::size_type pos = 0;
	while ( (pos = sTemplate.find(S1,pos)) != string::npos ) {

		string replaceto = remove2quote(arg1);
		replaceto = GET_OPCODE( replaceto );

		sTemplate.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}

	cout << "==================================================" << endl;
	cout << sTemplate << endl;
	cout << "==================================================" << endl;

	return sTemplate;
}


string NO_REG()
{
/*
	#define SIZE_A		8

    #define MAXARG_A        ((1<<SIZE_A)-1)

	#define NO_REG		MAXARG_A

*/
	string retS = "( (1<<8) - 1 )";

	return retS;
}

string SETARG_A(const string& arg1, const string& arg2)
{
	string sTemplate =
R"(
/*

#define SETARG_A(i,u)	((i) = (((i)&MASK0(SIZE_A,POS_A)) | \
		((cast(Instruction, u)<<POS_A)&MASK1(SIZE_A,POS_A))))

*/
)";

	string sTemplate_2 =
R"(
		 (__ARG__1) =   ( (__ARG__1) & (~((~((~(Instruction)0)<<8))<<6)) ) 
		              | ( ((__ARG__2) << 6 )      &   ((~((~(Instruction)0)<<8))<<6)  );
)";
	
	const string S1 = "__ARG__1";
	const string S2 = "__ARG__2";

	string::size_type pos = 0;
	while ( (pos = sTemplate_2.find(S1,pos)) != string::npos ) {

		string replaceto = remove2quote(arg1);

		sTemplate_2.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}

	pos = 0;
	while ( (pos = sTemplate_2.find(S2,pos)) != string::npos ) {
		string replaceto = remove2quote(arg2);

		sTemplate_2.replace( pos, S2.size(), replaceto);
		pos += replaceto.size();
	}

	if ( printFlag ) {
		cout << "==================================================" << endl;
		cout << (sTemplate + sTemplate_2) << endl;
		cout << "==================================================" << endl;
	}

	return sTemplate_2;
}


string CREATE_ABC(const string& arg1, const string arg2, const string& arg3, const string arg4)
{
	string sComment =
R"(
/*

	#define POS_OP		0
    #define POS_A		(POS_OP + SIZE_OP)  // 6
	#define POS_B		(POS_C + SIZE_C)   // (14+9)  = 23
	#define POS_C		(POS_A + SIZE_A)   // (6 + 8) = (14)

#define CREATE_ABC(o,a,b,c)	((cast(Instruction, o)<<POS_OP) \
			| (cast(Instruction, a)<<POS_A) \
			| (cast(Instruction, b)<<POS_B) \
			| (cast(Instruction, c)<<POS_C))
*/
)";

	string sTemplate =
R"(
	   (  
		  ( (Instruction)( (__ARG__1) << 0  ) )
	|     ( (Instruction)( (__ARG__2) << 6  ) )
	|     ( (Instruction)( (__ARG__3) << 23 ) )
	|     ( (Instruction)( (__ARG__4) << 14 ) )
       );
)";
	
	const string S1 = "__ARG__1";
	const string S2 = "__ARG__2";
	const string S3 = "__ARG__3";
	const string S4 = "__ARG__4";

	string::size_type pos = 0;
	while ( (pos = sTemplate.find(S1,pos)) != string::npos ) {
		string replaceto = remove2quote(arg1);
		sTemplate.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}

	pos = 0;
	while ( (pos = sTemplate.find(S2,pos)) != string::npos ) {
		string replaceto = remove2quote(arg2);
		sTemplate.replace( pos, S2.size(), replaceto);
		pos += replaceto.size();
	}

	pos = 0;
	while ( (pos = sTemplate.find(S3,pos)) != string::npos ) {
		string replaceto = remove2quote(arg3);
		sTemplate.replace( pos, S3.size(), replaceto);
		pos += replaceto.size();
	}

	pos = 0;
	while ( (pos = sTemplate.find(S4,pos)) != string::npos ) {
		string replaceto = remove2quote(arg4);
		sTemplate.replace( pos, S4.size(), replaceto);
		pos += replaceto.size();
	}
	
	if ( printFlag ) {
		cout << "==================================================" << endl;
		cout << (sComment + sTemplate) << endl;
		cout << "==================================================" << endl;
	}

	return sTemplate;
}

string GETARG_C(const string& arg1)
{
	string sComment =
R"(
/*
	#define POS_C		(POS_A + SIZE_A)   // (6 + 8) = (14)
	#define SIZE_C		9
	#define GETARG_C(i)	(cast(int, ((i)>>POS_C) & MASK1(SIZE_C,0)))
*/
)";

	string sTemplate = R"( ( (int)( ((__ARG__1) >> 14) & ((~((~(Instruction)0)<<9))<<0) ) ) )";
	
	const string S1 = "__ARG__1";

	string::size_type pos = 0;
	while ( (pos = sTemplate.find(S1,pos)) != string::npos ) {
		string replaceto = remove2quote(arg1);
		sTemplate.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}

	if ( printFlag ) {
		cout << "==================================================" << endl;
		cout << sComment + sTemplate << endl;
		cout << "==================================================" << endl;
	}

	return sTemplate;

}
string ISK(const string& arg1)
{
	string sComment =
R"(
/*
	#define SIZE_B		9
	#define BITRK		(1 << (9-1))   //  (1 << 8)
	#define ISK(x)		((x) & BITRK)
*/
)";

	string sTemplate = R"( ( (__ARG__1) & (1 << 8) ) )";
	
	const string S1 = "__ARG__1";

	string::size_type pos = 0;
	while ( (pos = sTemplate.find(S1,pos)) != string::npos ) {
		string replaceto = remove2quote(arg1);
		sTemplate.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}

	cout << "==================================================" << endl;
	cout << sTemplate << endl;
	cout << "==================================================" << endl;

	return sTemplate;

}

string SETARG_C(const string& arg1, const string& arg2)
{
	string sComment  = 
R"(
/*
 
	#define SIZE_C		9
	#define POS_C		(POS_A + SIZE_A)   // (6 + 8) = (14)

	#define SETARG_C(i,b)	((i) = (((i)&MASK0(SIZE_C,POS_C)) | \
			((cast(Instruction, b)<<POS_C)&MASK1(SIZE_C,POS_C))))
*/
)";

	string sTemplate = 
R"(
	(__ARG__1) = (    ( (__ARG__1)                              & (~( (~((~(Instruction)0)<<9))<<14 )) )
	               |  ( ( ( (Instruction)( __ARG__2 ) ) << 14 ) &   ( (~((~(Instruction)0)<<9))<<14 ) )
				 );
)";
	
	const string S1 = "__ARG__1";
	const string S2 = "__ARG__2";

	string::size_type pos = 0;
	while ( (pos = sTemplate.find(S1,pos)) != string::npos ) {
		string replaceto = remove2quote(arg1);
		sTemplate.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}

	pos = 0;
	while ( (pos = sTemplate.find(S2,pos)) != string::npos ) {
		string replaceto = remove2quote(arg2);
		sTemplate.replace( pos, S2.size(), replaceto);
		pos += replaceto.size();
	}

	if (  printFlag ) {
		cout << "==================================================" << endl;
		cout << (sComment + sTemplate) << endl;
		cout << "==================================================" << endl;
	}

	return sTemplate;

}

string getcode(const string& arg1, const string& arg2)
{
	string sComment  = 
R"(
/*
	#define getcode(fs,e)	((fs)->f->code[(e)->u.s.info])
*/
)";

	string sTemplate = R"( ( (__ARG__1)->f->code[ (__ARG__2)->u.s.info] ) )";
	
	const string S1 = "__ARG__1";
	const string S2 = "__ARG__2";

	string::size_type pos = 0;
	while ( (pos = sTemplate.find(S1,pos)) != string::npos ) {
		string replaceto = remove2quote(arg1);
		sTemplate.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}

	pos = 0;
	while ( (pos = sTemplate.find(S2,pos)) != string::npos ) {
		string replaceto = remove2quote(arg2);
		sTemplate.replace( pos, S2.size(), replaceto);
		pos += replaceto.size();
	}

	if ( printFlag ) {
		cout << "==================================================" << endl;
		cout << (sComment + sTemplate) << endl;
		cout << "==================================================" << endl;
	}

	return sTemplate;


}

string CREATE_ABx(const string& arg1, const string& arg2, const string& arg3)
{

	string sComment  = 
R"(
/*
	#define CREATE_ABx(o,a,bc)	((cast(Instruction, o)<<POS_OP) \
				| (cast(Instruction, a)<<POS_A) \
				| (cast(Instruction, bc)<<POS_Bx))

	#define POS_OP		0
    #define POS_A		(POS_OP + SIZE_OP)  // 6
	#define POS_Bx		POS_C              // 14

*/
)";
	string sTemplate =
R"(
	   ( ((Instruction)(__ARG__1)) << 0 ) 
	 | ( ((Instruction)(__ARG__2)) << 6 ) 
	 | ( ((Instruction)(__ARG__3)) << 14 ) 
)";
	
	const string S1 = "__ARG__1";
	const string S2 = "__ARG__2";
	const string S3 = "__ARG__3";

	string::size_type pos = 0;
	while ( (pos = sTemplate.find(S1,pos)) != string::npos ) {
		string replaceto = remove2quote(arg1);
		sTemplate.replace( pos, S1.size(), replaceto);
		pos += replaceto.size();
	}

	pos = 0;
	while ( (pos = sTemplate.find(S2,pos)) != string::npos ) {
		string replaceto = remove2quote(arg2);
		sTemplate.replace( pos, S2.size(), replaceto);
		pos += replaceto.size();
	}

	pos = 0;
	while ( (pos = sTemplate.find(S3,pos)) != string::npos ) {
		string replaceto = remove2quote(arg3);
		sTemplate.replace( pos, S3.size(), replaceto);
		pos += replaceto.size();
	}

	if ( printFlag ) {
		cout << "==================================================" << endl;
		cout << (sComment + sTemplate) << endl;
		cout << "==================================================" << endl;
	}

	return sTemplate;

}



int main (int argc, char* argv[], char* env[])
{
	// SETARG_sBx(argv[1], argv[2]);
	// GETARG_sBx( argv[1]);
	// GETARG_B(  argv[1] );
	// auto frep = CREATE_ABC(argv[1], GETARG_B(argv[2]), argv[3], GETARG_C(argv[4]) );
	// auto frep = SETARG_C( getcode(argv[1], argv[2]) , argv[3] );
	// auto frep = SETARG_B( getcode(argv[1], argv[2]), argv[3]);
	// auto frep = SETARG_A( getcode(argv[1], argv[2]), argv[3]);
	// auto frep = GETARG_A( getcode( argv[1], argv[2] ) );
	// auto frep = SETARG_B( getcode( argv[1], argv[2] ), argv[3] );
	// auto frep = GET_OPCODE( getcode( argv[1], argv[2] ) );
	
    // CREATE_ABx
	// auto frep = CREATE_ABC( argv[1], argv[2] , argv[3] , argv[4] );
	auto frep = CREATE_ABx( argv[1], argv[2] , argv[3] );
	cout << "============================== Final Ret 1  Begin ==============================" << endl;
	cout << frep << endl;
	cout << "============================== Final Ret 1 End ==============================" << endl;
	cout << endl;

	return 0;
}
