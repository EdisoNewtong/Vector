

void swap2Numbers(int* ary, int idx1, int idx2, int sz)
{
	if ( idx1 == idx2 ) {
		return;
	}

	int backupNum = ary[idx1];
	ary[idx1] = ary[idx2];
	ary[idx2] = backupNum;
}

void qSortSimpleVersion(int* ary, int begIdx, int endIdx, int sz)
{
	if ( endIdx <= begIdx ) {
		return;
	}

	int povitNumber = ary[begIdx];
	int i = begIdx + 1;
	int j = endIdx;
	while ( i < j ) 
	{
		for ( ; i < j && ary[i] <= povitNumber; ++i ) { ; }
		for ( ; j > i && ary[j] >= povitNumber; --j ) { ; }

		swap2Numbers(ary,i,j,sz);
		++i;
		--j;
	}

	int sepIdx = 0;
	if ( i == j ) {
		sepIdx = i;
	} else {
		// i > j
		sepIdx = i - 1;
	}

	if ( ary[sepIdx] >= povitNumber ) {
		sepIdx = sepIdx - 1;
	}

	swap2Numbers(ary,begIdx,sepIdx,sz);

	// sort left
	qSortSimpleVersion(ary, begIdx, sepIdx - 1, sz);
	// sort right
	qSortSimpleVersion(ary, sepIdx + 1, endIdx, sz);
}

bool checkIsSorted(int* ary,int sz)
{
	auto isSort = true;
	for ( auto i = 0; i < sz-1 ; ++i ) {
		if ( ary[i] > ary[i+1] ) {
			isSort = false;
			break;
		}
	}
	return isSort;
}

void testSort()
{
	const char* FILENAME = "1_5.txt";

	ifstream fobj(FILENAME ,ios::in);
	if ( !fobj ) {
		cout << "Can't open file " << FILENAME << endl;
		return;
	}

	int lineNo = 0;
	while ( !fobj.eof() ) 
	{
		string line;
		getline(fobj, line);
		if ( line.empty() ) {
			continue;
		}
		++lineNo;

		string::size_type beg = 0;
		string::size_type foundpos;

		list<int> lary;
		while( (foundpos = line.find(",",beg) ) != string::npos ) {
			string strnum = line.substr(beg, foundpos);
			int num = std::stoi( strnum );
			lary.push_back(num);
			beg = foundpos + 1;
		}

		// push rest one
		string reststr = line.substr(beg);
		int num = std::stoi( reststr );
		lary.push_back(num);

		int arysz = static_cast<int>( lary.size() );
		int* pbackupAry = new int[arysz];
		int* pary = new int[arysz];
		int idx = 0;
		for ( const auto& ele : lary  ) {
			pbackupAry[idx] = pary[idx] = ele;
			++idx;
		}

		cout << "Line " << setw(5) << lineNo << " : [ ";
		for ( int i = 0; i < arysz; ++i ) {
			cout << pbackupAry[i];
			if ( i < arysz-1 ) {
				cout << ", ";
			}
		}
		cout << " ]    "; // << endl;

		qSortSimpleVersion(pary, 0, arysz-1, arysz);
		auto bIsSorted = checkIsSorted( pary, arysz);
		if ( bIsSorted ) {
			cout << "=> Sort SUCC" << endl;
		} else {
			cout << "=> Sort Failed   :( " << endl;
			cout << "After Sort : " << endl;
			cout << "\t [ ";
			for ( int i = 0; i < arysz; ++i ) {
				cout << pary[i];
				if ( i < arysz-1 ) {
					cout << ", ";
				}
			}
			cout << " ]    " << endl;
		}

		delete [] pbackupAry;
		delete [] pary;
	}

	fobj.close();
}

