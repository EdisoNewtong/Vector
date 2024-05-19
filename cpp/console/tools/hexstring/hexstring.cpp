#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;


void printUsgae()
{
    cout << "Usage : " << endl;
    cout << "       hexstring <inputFile>" << endl;
    cout << "or     hexstring [ -o <outputFile> ] <inputFile>" << endl;
}


int main(int argc,char* argv[], char* env[])
{
    if ( argc < 2 ) {
        printUsgae();
        return -1;
    }

    const string fixedarg1("-o");

    // argc >= 2
    string arg1( argv[1] );
	char* fileBufArray = nullptr; 

    if ( arg1 == fixedarg1 ) {
        if ( argc != 4 ) {
            printUsgae();
            return -1;
        }

        // == 4
        string arg2( argv[2] ); // -o  <Output File>
        string arg3( argv[3] ); // input file
        
        ifstream inputfile(arg3.c_str() , ios::in | ios::binary);
        if ( !inputfile ) {
            cout << "[ERROR] Can't open file  : " << arg3 << "  or file is not existed  :(" << endl;
            return -1;
        }

        if ( arg2 == arg3 ) {
            inputfile.close();
            cout << "[ERROR] Specifying same input and output file is not allowed :(" << endl;
            return -1;
        }

        
        ifstream testOutputfile(arg2.c_str() , ios::in | ios::binary);
        if ( testOutputfile ) {
            cout << "[ERROR] File : " << arg2 << " is already existed ! Please specify a new file name. " << endl;
            return -1;
        }
        testOutputfile.close();

        ofstream outputfile( arg2.c_str(), ios::out | ios::trunc | ios::binary);
        if ( !outputfile ) {
            cout << "[ERROR] Can't open file : " << arg2 << "  or permission deny " << endl;
            return -1;
        }

        // get the input file's length
        inputfile.seekg(0, ios::end);
        size_t filelen = static_cast<int>( inputfile.tellg() );
        inputfile.seekg(0, ios::beg);

        if ( filelen > 0 ) {
			fileBufArray = new char[filelen];
			if ( fileBufArray == nullptr ) {
				inputfile.close();
				outputfile.flush();
				outputfile.close();
				cout << "[ERROR] Can't read file content into file-Buffer." << endl;
				return -1;
			} else {
				outputfile << "\"";
			}

			inputfile.read(fileBufArray, filelen);
            inputfile.close();

            auto lastIsEOLFlag = false;
            for ( size_t idx = 0; idx < filelen; ) {
                char ch = fileBufArray[idx];

				if ( ch == '\r' ) {
					size_t nextIdx = idx + 1;
					if ( nextIdx  < filelen ) {
						if ( fileBufArray[nextIdx] == '\n' ) {
							// \r \n
                            outputfile << "\"\n";
                            if ( (nextIdx+1) < filelen ) {
                                cout << "\"";
                            } else {
                                lastIsEOLFlag = true;
                            }
                            idx += 2;
						} else {
							// \r a
                            outputfile << "\"\n\"";
                            ++idx;
						}
					} else {
						// \r is the last character
                        // do nothing
                        outputfile << "\"\n";
                        lastIsEOLFlag = true;

                        ++idx;
					}
				} else if ( ch == '\n' ) {
                    outputfile << "\"\n";
                    if ( (idx+1) < filelen ) {
                        outputfile << "\"";
                    } else {
                        lastIsEOLFlag = true;
                    }

                    ++idx;
				} else {
					outputfile << "\\x" << std::hex << setw(2) << setfill('0') << std::uppercase << static_cast<int>(ch & 0xFFU);

                    ++idx;
				}
            }

            if ( !lastIsEOLFlag ) {
                outputfile << "\"";
            }
        }


        outputfile.flush();
        outputfile.close();
        cout << "[DONE] hexstring result is written into file : " << arg2 << endl;
    } else {
        // !=  "-o" , treat as       "hexstring <inputFile>"   command format
        ifstream inputfile(arg1.c_str() , ios::in | ios::binary);
        if ( !inputfile ) {
            cout << "[ERROR] Can't open file  : " << arg1 << "  or file is not existed  :(" << endl;
            return -1;
        }

        // file is existed , output convert result to console
        inputfile.seekg(0, ios::end);
        size_t filelen = static_cast<int>( inputfile.tellg() );
        inputfile.seekg(0, ios::beg);

        // cout << "file.length = " <<  filelen << endl;

        ///////////////////////////////////////////////////////////////////////////
        // Begin
        if ( filelen > 0 ) {
			fileBufArray = new char[filelen];
			if ( fileBufArray == nullptr ) {
				inputfile.close();
				cout << "[ERROR] Can't read file content into file-Buffer." << endl;
				return -1;
			} else {
				cout << "\"";
			}

			inputfile.read(fileBufArray, filelen);
			inputfile.close();

            auto lastIsEOLFlag = false;
            for ( size_t idx = 0; idx < filelen; ) {
                char ch = fileBufArray[idx];

				if ( ch == '\r' ) {
					size_t nextIdx = idx + 1;
					if ( nextIdx  < filelen ) {
						if ( fileBufArray[nextIdx] == '\n' ) {
							// \r \n
                            cout << "\"\n";
                            if ( (nextIdx+1) < filelen ) {
                                cout << "\"";
                            } else {
                                lastIsEOLFlag = true;
                            }
                            idx += 2;
						} else {
							// \r a
                            cout << "\"\n\"";
                            ++idx;
						}
					} else {
						// \r is the last character
                        // do nothing
                        cout << "\"\n";
                        lastIsEOLFlag = true;

                        ++idx;
					}
				} else if ( ch == '\n' ) {
                    cout << "\"\n";
                    if ( (idx+1) < filelen ) {
                        cout << "\"";
                    } else {
                        lastIsEOLFlag = true;
                    }

                    ++idx;
				} else {
					cout << "\\x" << std::hex << setw(2) << setfill('0') << std::uppercase << static_cast<int>(ch & 0xFFU);

                    ++idx;
				}
            }

            if ( !lastIsEOLFlag  ) {
                cout << "\"";
            }
        }

        // End
        ///////////////////////////////////////////////////////////////////////////
    }


	if ( fileBufArray != nullptr ) {
		delete [] fileBufArray;
		fileBufArray = nullptr;
	}

    return 0;
}
