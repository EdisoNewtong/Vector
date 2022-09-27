#include <iostream>
#include <iomanip>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <iterator>
#include <set>


#include "Removing.h"
using namespace std;

static const char G_TAB = '\t';


namespace Removing
{
    


void testCaseAll()
{    
	cout << "######################### Removing Algorithm Begin #########################" << endl;
	removeCase();
	remove_copyCase();
	uniqueCase();
	unique_copyCase();
    cout << "######################### Removing Algorithm End   #########################" << endl;
}



void removeCase()
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Core Core Core :
	//                  remove(...) algorithm do not really drop a certain element out of the container
	//                  *****************  You need to call *****************
	//						    container.erase(...) to remove them
	//                  ***************************************************
    cout << G_TAB << "--------------- remove(...) / remove_if(...) --------------- " << endl;
	vector<int> v;
	for( int n = 2; n <= 6; ++n ) { v.push_back(n); }
	for( int n = 4; n <= 9; ++n ) { v.push_back(n); }
	for( int n = 1; n <= 7; ++n ) { v.push_back(n); }

    // print original
    cout << G_TAB << "Original vector v                      : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] . v.size() = " << v.size() << endl;

	vector<int>::iterator pos;
	pos = remove( v.begin(), v.end(), 5); // remove the elements whose value is 5
    cout << G_TAB << "size not changed                       : [ ";
	int idx = 0;
    for( const auto& e : v ) {
		if ( idx == 3 || idx == 5 || idx == 13 ) {
			cout << "  ";
		}
        cout << e << " ";
		++idx;
    }
    cout << " ] . v.size() = " << v.size() << endl;

	// Core Core Core :  call     v.erase(pos , end); to really remove thme out 
	v.erase( pos, v.end() );
    cout << G_TAB << "size     changed                       : [ ";
	idx = 0;
    for( const auto& e : v ) {
		if ( idx == 3 || idx == 5 || idx == 13 ) {
			cout << "  ";
		}
        cout << e << " ";
		++idx;
    }
    // copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] . v.size() = " << v.size() << endl;


	v.erase( remove_if(v.begin(), v.end(),
				       [](int ele) -> bool { // remove criterion func
							return ele < 4;
					   }),
			 v.end() );
    cout << G_TAB << "remove elements < 4                    : [ ";
    copy( v.begin(), v.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] . v.size() = " << v.size() << endl;


    // cout << G_TAB << "********************************************************************************" << endl;
	
    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
	
}

void remove_copyCase()
{
    cout << G_TAB << "--------------- remove_copy(...) / remove_copy_if(...) --------------- " << endl;
	vector<int> v1;
	vector<int> v2;
	multiset<int> s3;
	for( int n = 1; n <= 6; ++n ) { v1.push_back(n); }
	for( int n = 1; n <= 9; ++n ) { v1.push_back(n); }

    // print original
    cout << G_TAB << "Original vector v1                      : [ ";
    copy( v1.begin(), v1.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

	// remove all elements whose value is 3 And copy the rest into the container : v2
	remove_copy(v1.cbegin(), v1.cend(), back_inserter(v2), 3);
    cout << G_TAB << "After remove_copy(...) vector v2        : [ ";
    copy( v2.cbegin(), v2.cend(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

	v2.clear();
	// copy elements whose value is <=4 into    v2
	remove_copy_if(v1.cbegin(), v1.cend(), back_inserter(v2), 
			    [](int ele) {
				    return  ele > 4;
				});
    cout << G_TAB << "After remove_copy_if(...) vector v2     : [ ";
    copy( v2.begin(), v2.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

	

	remove_copy_if(v1.cbegin(), v1.cend(), inserter(s3, s3.end()),
				   // remove  ele < 4 ,  copy ele >= 4 into s3
			       bind( less<int>(), std::placeholders::_1,  4) );
    cout << G_TAB << "After remove_copy_if(...) multiset s3   : [ ";
    copy( s3.begin(), s3.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;



    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}

void uniqueCase()
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Core Core Core :
    // Remove  !!!<Consecutive>!!!   Duplicates  elements
	//
    cout << G_TAB << "--------------- unique(...) --------------- " << endl;
	vector<int> v0{  1, 4, 4,         // 1st:  2 duplicate of 4
		             6, 1, 2, 2,      // 2nd:  2 duplicate of 2   | Notes : the duplicate 1 both in 1st and 2nd , Do <Not> removed
					 3, 1, 6, 6, 6,   // 3rd:  3 duplicate of 6   | Notes : the duplicate 1 among  1st and 2nd and 3rd , 6 in both 2nd and 3rd , Do <Not> removed
					 5, 7, 5, 4, 4    // 4th:  2 duplicate of 4   | Notes : the duplicate 1 among  1st and 2nd and 3rd , 6 in both 2nd and 3rd , 4 in both 1st and 4th , Do <Not> removed
				  };
	list<int> v1;
	list<int> v2;
	copy( v0.begin(), v0.end(), back_inserter(v1) );

    cout << G_TAB << "Original vector v1                      : [ ";
    copy( v1.begin(), v1.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Core Core Core :
	//		remove <Consecutive> duplicates
	auto pos = unique( v1.begin(), v1.end() );
	copy(v1.begin(), pos, back_inserter(v2) );
    cout << G_TAB << "after unique v1 --> v2                  : [ ";
    copy( v2.begin(), v2.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;


	///////////////////////////////////////////////////////////////////////////////////
	//  to be deleted :
	//
    //                    {             1, 2, 2, 3, 1,            5,       5, 4, 4    }
	//                    { 1, 4, 4, 6, 1, 2, 2, 3, 1,   6, 6, 6, 5,    7, 5, 4, 4    }
	//                               ----------------         ------    -----------
	//                    { 1, 4, 4, 6                   6, 6, 6,       7 }
    copy( v0.begin(), v0.end(), v1.begin() );
	v1.erase( 
			  unique( v1.begin(), v1.end(), greater<int>() ),   // begin
		      v1.end() ); // end

    cout << G_TAB << "after 2nd unique v1                     : [ ";
    copy( v1.begin(), v1.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}




void unique_copyCase()
{
    cout << G_TAB << "--------------- unique_copy(...) --------------- " << endl;
	vector<int> v0{  1, 4, 4,         // 1st:  2 duplicate of 4
		             6, 1, 2, 2,      // 2nd:  2 duplicate of 2   | Notes : the duplicate 1 both in 1st and 2nd , Do <Not> removed
					 3, 1, 6, 6, 6,   // 3rd:  3 duplicate of 6   | Notes : the duplicate 1 among  1st and 2nd and 3rd , 6 in both 2nd and 3rd , Do <Not> removed
					 5, 7, 5, 4, 4    // 4th:  2 duplicate of 4   | Notes : the duplicate 1 among  1st and 2nd and 3rd , 6 in both 2nd and 3rd , 4 in both 1st and 4th , Do <Not> removed
				  };
	vector<int> v1;
	vector<int> v2;

	copy( v0.begin(), v0.end(), back_inserter(v1) );
    cout << G_TAB << "Original vector v1                      : [ ";
    copy( v1.begin(), v1.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

	unique_copy( v1.cbegin(), v1.cend(), back_inserter(v2) );
    cout << G_TAB << "after unique v1 --> v2                  : [ ";
    copy( v2.begin(), v2.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;


	auto differenceOne = [](int e1, int e2) -> bool {
		return (e1+1 == e2) || (e1-1 == e2);
	};

	v2.clear();
	unique_copy( v1.cbegin(), v1.cend(), back_inserter(v2),  
			differenceOne );  // criterion func

    cout << G_TAB << "after 2nd unique v1 --> v2              : [ ";
    copy( v2.begin(), v2.end(), ostream_iterator<int>(cout, " ") );
    cout << " ] " << endl;

    cout << G_TAB << "------------------------------------------------------------" << endl << endl;
}




}
