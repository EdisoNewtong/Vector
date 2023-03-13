#include <iostream>
#include <iomanip>
#include <vector>
#include <list>
#include <set>
#include <algorithm>
#include <iterator>
#include <random> // for  class default_random_engine    Use


#include "Mutating.h"
using namespace std;

static const char G_TAB = '\t';

/*******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
                        

                        ----------------
    Mutating algorithms change the order of elements but not their values
                        ----------------
						^^^^^^^^^^^^^^^^

********************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************/
namespace Mutating
{
    


void testCaseAll()
{
	cout << "######################### Mutating Algorithm Begin #########################" << endl;
	reverseCase();
	rotateCase();
	rotate_copyCase();
	permutationCase();
	shuffleCase();
	partitionCase();
    cout << "######################### Mutating Algorithm End   #########################" << endl;

}

void reverseCase()
{
    cout << G_TAB << "--------------- reverse(...) / reverse_copy(...) --------------- " << endl;
	vector<int> v;
	for( int n = 1; n <= 9; ++n ) { v.push_back(n); }

    // print original
    cout << G_TAB << "Original vector v                      : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

	reverse( v.begin(), v.end() );
    cout << G_TAB << "Reverse  vector v                      : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

	reverse( v.begin()+1, v.end()-1 );
    cout << G_TAB << "Reverse  vector v (beg+1,end-1)        : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;


    // cout << G_TAB << "********************************************************************************" << endl;

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}



void rotateCase()
{
    cout << G_TAB << "--------------- rotate(...)  --------------- " << endl;
	/*
	    << n  :  <LEFT>  shift n step :   v.begin() + n      <=>  v.end()   - ( v.size() - n )
		>> n  :  <RIGHT> shift n step :   v.end()   - n      <=>  v.begin() + ( v.size() - n )
	*/
	vector<int> v;
	for( int n = 1; n <= 9; ++n ) { v.push_back(n); }

    // print original
    cout << G_TAB << "Original vector v                      : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

	// <<  1
	// rotate one element to the <Left>    
	rotate(                  v.begin(),      // beginning of range  
	/* new first element */  v.begin() + 1,     
			                 v.end() );      // end of range
    cout << G_TAB << " << 1  one left | vector v             : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

	// >> 2
	// rotate two elements to the <Right>    
	rotate(                  v.begin(),      // beginning of range  
	/* new first element */  v.end() - 2,     
			                 v.end() );      // end of range
    cout << G_TAB << " >> 2  two right | vector v            : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

	// >> 2
	// rotate two elements to the <Right>    
	rotate(                  v.begin(),      // beginning of range  
	/* new first element */  find(v.begin(), v.end(), 4),
			                 v.end() );      // end of range
    cout << G_TAB << " let 4 placed at 1st | vector v        : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;


    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}



void rotate_copyCase()
{
    cout << G_TAB << "--------------- rotate_copy(...)  --------------- " << endl;
	set<int> st;
	for( int n = 1; n <= 9; ++n ) { st.insert(n); }

    // print original
    cout << G_TAB << "Original set                        : [ ";
    copy( st.begin(), st.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;


	set<int>::iterator pos = st.begin();
	++pos,
    cout << G_TAB << "Left Shift 1 step set               : [ ";
	rotate_copy( st.begin(), 
			     pos,
				 st.end(),
				 ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;


	pos = st.end();
	advance(pos, -2);
    cout << G_TAB << "Right Shift 2 step set              : [ ";
	rotate_copy( st.begin(), 
			     pos,
				 st.end(),
				 ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;



    cout << G_TAB << "let 4 placed at 1st set             : [ ";
	rotate_copy( st.begin(), 
				 st.find(4),	     
				 st.end(),
				 ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;


    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}





void permutationCase()
{
    cout << G_TAB << "--------------- next_permutation(...) / prev_permutation(...)  --------------- " << endl;
	vector<int> v;
	for( int n = 1; n <= 4; ++n ) { v.push_back(n); }

    // print original
    cout << G_TAB << "Original vector   : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

	while ( next_permutation( v.begin(), v.end() ) ) {
		cout << G_TAB << "    next permutation  : [ ";
		copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
		cout << " ] " << endl;
	}

    cout << G_TAB << "Afterward vector  : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

	cout << endl;
    cout << G_TAB << "********************************************************************************" << endl << endl;

	while( prev_permutation( v.begin(), v.end() ) ) {
		cout << G_TAB << "    prev permutation  : [ ";
		copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
		cout << " ] " << endl;
	}
    cout << G_TAB << "Now       vector  : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;


	while( prev_permutation( v.begin(), v.end() ) ) {
		cout << G_TAB << "    prev permutation  : [ ";
		copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
		cout << " ] " << endl;
	}
    cout << G_TAB << "Afterward vector  : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;
	

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}


class MyRandom {
public:
	ptrdiff_t operator() (ptrdiff_t max) {
		/*
		    max is the size of the increased range()
			e.g.   
					vector<int> v{ 1,2,3,4,5,6 }

		--------------------------------------------------
			1. max = 2
			2. max = 3
			3. max = 4
			4. max = 5
			5. max = 6
		--------------------------------------------------

		*/
		// static int g_CNT = 0;
		// ++g_CNT;
		// cout << g_CNT << ".  max = " << max << endl;

		double tmp =    static_cast<double>( rand() ) 
			         /  static_cast<double>( RAND_MAX );
		return static_cast<ptrdiff_t>( tmp * max);
	}
};


void shuffleCase()
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//  void shuffle(RandomAccessIterator beg, RandomAccessIterator end,
	//                UniformRandomNumberGenerator&& eng);
	//
	//
	//  void random_shuffle(RandomAccessIterator beg, RandomAccessIterator end);
	//
	//
	//  void random_shuffle(RandomAccessIterator beg, RandomAccessIterator end,
	//                      RandomFunc&& op);
	//
    cout << G_TAB << "--------------- shuffle(...) / random_suffle(...) --------------- " << endl;
	vector<int> v{ 1, 2, 3, 4, 5, 6, 7, 8 , 9 };
	
    // print original
    cout << G_TAB << "Original       vector                                 : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;


	random_shuffle( v.begin(), v.end() );
    cout << G_TAB << "random_shuffle vector 1                               : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;


	// sort them again
	sort( v.begin(), v.end() );
    cout << G_TAB << "Sorted         vector                                 : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

	default_random_engine eng;
	shuffle( v.begin(), v.end() , eng );
    cout << G_TAB << "Shuffled       vector 2                               : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;


	cout << endl;
    cout << G_TAB << "********************************************************************************" << endl << endl;

	MyRandom randomObject;

	sort( v.begin(), v.end() );
    cout << G_TAB << "Sorted         vector                                 : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

	random_shuffle( v.begin(), v.end(), randomObject);
    cout << G_TAB << "random_shuffle ( Self-Defined randomObject ) vector 3 : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl << endl;



    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}




void partitionCase()
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//  void partition(ForwardIterator beg, ForwardIterator end,  UnaryPredicate op);
	//                
    cout << G_TAB << "--------------- partition(...) / stable_partition(...) / partition_copy(...) --------------- " << endl;
	vector<int> v1{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	vector<int> v2{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    cout << G_TAB << "Original          vector v1   : [ ";
    copy( v1.begin(), v1.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;


	vector<int>::iterator pos1;
	vector<int>::iterator pos2;
	int idx = 0;

	////////////////////////////////////////////////////////////////
	//
	// seperate into 2 parts :  
	//
	//		left part       ||   right part
	//------------------------------------------------
	//      even numbers    ||   odd numbers
	//
	pos1 = partition( v1.begin(), v1.end(),
			          [](int ele) -> bool {
						  return ele % 2 == 0;
					  } );

    cout << G_TAB << "Partition         vector v1   : [ ";
	idx = 0;
	for( const auto& e : v1 ) {
		if ( idx == 4 ) {
			cout << "    ";
		}
		cout << e << " ";
		++idx;
	}
    // copy( v1.begin(), v1.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;
	cout << G_TAB << "first odd element of     v1   :               " << *pos1 << endl;

	cout << endl;
    cout << G_TAB << "********************************************************************************" << endl << endl;




    cout << G_TAB << "Original          vector v2   : [ ";
    copy( v2.begin(), v2.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

	pos2 = stable_partition( v2.begin(), v2.end(),
			          [](int ele) -> bool {
						  return ele % 2 == 0;
					  } );

    cout << G_TAB << "Statle_Partition  vector v2   : [ ";
	idx = 0;
	for( const auto& e : v2 ) {
		if ( idx == 4 ) {
			cout << "  ";
		}
		cout << e << " ";
		++idx;
	}
    // copy( v2.begin(), v2.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;
	cout << G_TAB << "first odd element of     v2   :             " << *pos2 << endl;


	cout << endl;
    cout << G_TAB << "********************************************************************************" << endl << endl;

	vector<int> v3{ 1, 6, 33, 7, 22, 4, 11, 33, 2, 7, 0, 42, 5 };
    cout << G_TAB << "Original          vector v3   : [ ";
    copy( v3.begin(), v3.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

	vector<int> vEven;
	vector<int> vOdd;

	partition_copy(v3.begin(), v3.end(),
			       back_inserter(vEven),  // ele % 2 == 0
			       back_inserter(vOdd),   // ele % 2 == 1
				   [](int ele ) -> bool {
						return ele % 2 == 0;
				   } );

	cout << endl;
	cout << G_TAB << "-------------------- v3 is !!! <Unchangable>!!! after --------------------" << endl
		 << endl;
	
    cout << G_TAB << "partition_copy    vector v3   : [ ";
    copy( v3.begin(), v3.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    cout << G_TAB << "Even              vector      : [ ";
    copy( vEven.begin(), vEven.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    cout << G_TAB << "Odd               vector      : [ ";
    copy( vOdd.begin(), vOdd.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

	


    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}






}