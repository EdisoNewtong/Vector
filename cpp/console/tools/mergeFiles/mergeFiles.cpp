#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

static const string g_cstrPrefix("--output=");


void releaseVectorMem(vector< pair<string,ifstream*> >* pInputFileList)
{
    if ( pInputFileList != nullptr ) {
        for( auto it = pInputFileList->begin(); it != pInputFileList->end(); ++it ) {
            if ( it->second != nullptr ) {
                delete it->second;
                it->second = nullptr;
            }
        }

        pInputFileList->clear();
    }
}


void printUsage()
{
    const string C_STR_USAGE = R"(Usage :
	// at least 2 input files to merge
	$ mergeFile --output=<filename>   <file1>  <file2>  [file3 file4 file5 ... fileN])";
    cout << C_STR_USAGE << endl;
}


/************************************************************

// at least 2 input files to merge
$ mergeFile --output=<filename>   <file1>  <file2>  [file3 file4 file5 ... fileN]

*************************************************************/
int main(int argc, char* argv[])
{
    if ( argc < 4 ) {
        printUsage();
        return -1;
    }

    string arg1( argv[1] );
    if ( arg1.find(g_cstrPrefix) != 0 ) {
        cout << "[ERROR] Unknown argument 1 ." << endl;
        printUsage();
        return -1;
    }

    string outputFileName = arg1.substr( g_cstrPrefix.size() );
    vector< pair<string,ifstream*> > inputFileList;
    bool isAllFileExisted = true;
    for( int i = 2; i < argc; ++i ) {
        string fname(argv[i]);
        ifstream*  pInputfile = new ifstream(fname.c_str(), ios::in | ios::binary);
        if ( !(*pInputfile) ) {
            cout << "[ERROR] File \"" << fname << "\" is not existed or open failed. " << endl;
            isAllFileExisted = false;
            break;
        }

        inputFileList.push_back( make_pair(fname, pInputfile) );
    }

    if ( !isAllFileExisted ) {
        releaseVectorMem( &inputFileList );
        return -1;
    }

    //
    // All file is existed
    //
    bool hasSameFile = false;
    for( auto it = inputFileList.begin(); it != inputFileList.end(); ++it ) {
        if ( it->first == outputFileName ) {
            hasSameFile = true;
            break;
        }
    }

    if ( hasSameFile ) {
        cout << "[ERROR] The input file has the same filename with the outputfile. " << endl;
        releaseVectorMem( &inputFileList );
        return -1;
    }

    ofstream outfile(outputFileName.c_str(), ios::out | ios::trunc | ios::binary);
    if ( !outfile ) {
        cout << "[ERROR] The output file can't be opend. " << endl;
        releaseVectorMem( &inputFileList );
    }

    int fileIdx = 0;
    for( auto it = inputFileList.begin(); it != inputFileList.end(); ++it, ++fileIdx ) {
        auto pInputFileToMerge =  it->second;
        if ( pInputFileToMerge != nullptr ) {
            pInputFileToMerge->seekg(0, ios::end);
	        size_t fileSz = static_cast<size_t>( pInputFileToMerge->tellg() );
            pInputFileToMerge->seekg(0, ios::beg);

            for( size_t i = 0; i < fileSz; ++i ) {
                char chByte = 0;
                pInputFileToMerge->read( &chByte, sizeof(chByte) );
                outfile.write( &chByte, sizeof(chByte) );
            }
            pInputFileToMerge->close(); // close input

            outfile.flush();
            cout << "[INFO] : File-" << (fileIdx+1) << " : \"" << it->first << "\" has been merge into output file. " << endl;
        }
    }

    outfile.flush();
    outfile.close();
    cout << "[Done] All input files has been merged into outputfile : " << outputFileName << endl;

    releaseVectorMem( &inputFileList );
    return 0;
}

