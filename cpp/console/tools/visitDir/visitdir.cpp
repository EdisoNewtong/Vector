#include <iostream>
#include <iomanip>
#include <string>
#include <filesystem>   // A compiler which support C++17 standard is required 
#include <map>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

/********************************************************************************************************************************
  The following 9 characters are not allowed to be existed in a valid file's name 
    \ / : * ? " < > |  
********************************************************************************************************************************/


static const std::string SC_EMPTY_SUFFIX = "<Empty>";
static const std::string SC_DOT_SUFFIX   = "<Last_Dot>";
static const        char SC_DOT          = '.';

struct file_info {
    std::string abspath;
    std::string filename;
    bool bIsSymlink;

    file_info(const std::string& abs, const std::string& fName, bool isSym)
        : abspath( abs )
        , filename( fName )
        , bIsSymlink( isSym )
    { }
};

struct ext_list {
    map<string, vector<file_info> >   multiext_fileList;
                vector<file_info>    singleext_fileList;
};

int getDigitCount(size_t cnt)
{
    int retCnt = 0;
    do {
        ++retCnt;
        cnt /= 10;
    } while( cnt > 0 );
    return retCnt;
}


void analyzePath(const std::filesystem::path& absPath, map<string,ext_list>& mp,  bool bIsSym)
{
    std::string fileNameOnly = absPath.filename().string();
    std::string::size_type frontDotPos = fileNameOnly.find(SC_DOT);
    std::string::size_type lastDotPos;

    std::string completeSuffix;
    std::string lastSuffix;
    if ( frontDotPos != std::string::npos ) {
        lastDotPos = fileNameOnly.rfind(SC_DOT);
        if ( frontDotPos == lastDotPos ) {
            // Same   Suffix && completeSuffix
            completeSuffix = lastSuffix = fileNameOnly.substr( frontDotPos );
        } else {
            // different
            completeSuffix = fileNameOnly.substr( frontDotPos );
            lastSuffix     = fileNameOnly.substr( lastDotPos );
        }
    } 
    // else  completeSuffix && lastSuffix  become   <Empty>


    int suffixFlag = -1;
    std::string suffix2find;
    if ( lastSuffix.empty() ) {
        // <EMPTY Suffix>  e.g.   "~/work/src/cpp/testOnly/helloworld"
        suffixFlag = 0;
        suffix2find = SC_EMPTY_SUFFIX;
    } else if ( lastSuffix.size() == 1 && lastSuffix[0] == SC_DOT ) {
        // <DOT Suffix>    e.g.   "~/work/src/cpp/testOnly/a."  or  "~/work/src/cpp/testOnly/a.b.c."
        suffixFlag = 1;
        suffix2find = SC_DOT_SUFFIX;
    } else {
        suffixFlag = 2;
        suffix2find = lastSuffix;
    }


    file_info fInfo(absPath.string(), fileNameOnly, bIsSym);

    auto it = mp.find( suffix2find );
    if ( it == mp.end() ) {
        // insert one
        ext_list newList;
        if ( suffixFlag == 0 ) {
            newList.singleext_fileList.push_back( fInfo );
        } else {
            if ( completeSuffix == lastSuffix ) {
                newList.singleext_fileList.push_back( fInfo );
            } else {
                vector<file_info> vec; vec.push_back( fInfo );
                newList.multiext_fileList.insert( make_pair( completeSuffix , vec) );
            }
        }

        mp.insert( make_pair( suffix2find, newList) );
    } else {
        // update existed pairs
        if ( suffixFlag == 0 ) {
            it->second.singleext_fileList.push_back( fInfo );
        } else {
            if ( completeSuffix == lastSuffix ) {
                it->second.singleext_fileList.push_back( fInfo );
            } else {
                auto subIt = it->second.multiext_fileList.find( completeSuffix );
                if ( subIt == it->second.multiext_fileList.end() ) {
                    vector<file_info> vec; vec.push_back( fInfo );
                    it->second.multiext_fileList.insert( make_pair( completeSuffix , vec ) );
                } else {
                    subIt->second.push_back( fInfo );
                }
            }
        }
    }



}


void testOnly()
{
    // /*******************************************************************************************************
    string nameAry[] = {
        string("/a/b/.txt"),
        string("/a/b/.txt."),
        string("/a/b/.txt.back"),
        string("/a/b/filename"),
        string("/a/b/filename."),
        string("/a/b/filename.txt"),
        string("/a/b/filename.txt."),
        string("/a/b/filename.txt.back"),
    };


    int idx = 1;
    for( const auto& s : nameAry ) {
        fs::path ph( s.c_str() );
        auto fName = ph.filename();
        auto fext  = ph.extension();


        std::string finalext;
        int extCnt = 0;
        for ( fs::path pp = ph; !pp.extension().empty(); pp = pp.stem(), ++extCnt ) {
            // std::string part = pp.extension().string();
            finalext.insert(0, pp.extension().string() ); // part);
        }

        cout << idx << ".  path = \"" << s << "\"  # filename = " << fName <<  ", ext = |" << fext  << "|" << ", comExt = " <<  finalext << " ( " <<extCnt<< " )" <<endl;
        ++idx;
    }

    // *******************************************************************************************************/
}


int main(int argc, char* argv[], char* env[])
{
    // testOnly();
    // return -1;

    if ( argc < 2 ) {
        cout << "[ERROR] : Require at least one directory path. " << endl;
        return -1;
    }

    fs::path dir = argv[1];

    if ( !fs::is_directory(dir)  ) {
        cout << "[ERROR] : The given path is not a valid directory. " << endl;
        return -1;
    }

	map<int, pair<string, unsigned long long> > mp;
	mp.insert( make_pair(0 , make_pair("Dir", 0ull ) ) );
	mp.insert( make_pair(1 , make_pair("Regular File", 0ull ) ) );
	mp.insert( make_pair(2 , make_pair("Sym Link File", 0ull ) ) );
	mp.insert( make_pair(3 , make_pair("Block File", 0ull ) ) );
	mp.insert( make_pair(4 , make_pair("Character File", 0ull ) ) );
	mp.insert( make_pair(5 , make_pair("Named Pipe File", 0ull ) ) );
	mp.insert( make_pair(6 , make_pair("Socket File", 0ull ) ) );
	mp.insert( make_pair(7 , make_pair("Other File", 0ull ) ) );

    int max_dirDepth  = 0ull;
    int max_fileDepth = 0ull;

    // int G_FILE_CNT = 0;
    // Core Core Core : Data
    map<string,ext_list> fileExtensionMap;
    for ( auto it  = fs::recursive_directory_iterator(dir); 
               it != fs::recursive_directory_iterator();
             ++it 
        )
    {
        const auto& entry = *it;
		if ( entry.is_directory() ) {
			++mp[0].second;

            auto d = it.depth();
            if ( d > max_dirDepth ) {
                max_dirDepth = d;
            }
		} else if ( entry.is_regular_file() ) {
            // cout << ++G_FILE_CNT << ". " << entry.path() << endl;
			++mp[1].second;

            auto d = it.depth();
            if ( d > max_fileDepth ) {
                max_fileDepth = d;
            }

            analyzePath( entry.path(), fileExtensionMap, false );
		} else if ( entry.is_symlink() ) {
            // cout << ++G_FILE_CNT << ". (S) " << entry.path() << endl;
			++mp[2].second;

            auto d = it.depth();
            if ( d > max_fileDepth ) {
                max_fileDepth = d;
            }

            analyzePath( entry.path(), fileExtensionMap, true );
		} else if ( entry.is_block_file() ) {
			++mp[3].second;
		} else if ( entry.is_character_file() ) {
			++mp[4].second;
		} else if ( entry.is_fifo() ) {
			++mp[5].second;
		} else if ( entry.is_socket() ) {
			++mp[6].second;
		} else if ( entry.is_other() ) {
			++mp[7].second;
		} 

    }

    cout << "----------------------------------------" << endl;
    cout << "Summary of Dir:  \"" << dir << "\"" << endl;
    cout << "----------------------------------------" << endl;
	for( const auto& pr : mp ) {
        int type = pr.first;
		if ( type >=0 && type <= 2 ) {
            if ( type == 0  ) {
                // Dirs
                cout << "===========================================================================" << endl;
                cout << "Dir Count = " << mp[type].second << endl;
                cout << "Max Dir Depth = " << max_dirDepth << endl;
                cout << "===========================================================================" << endl;
            } else if ( type == 1 ) {
                // regular files and Symbol-Link files
                // cout << "===========================================================================" << endl;
                unsigned long long totolFileCnt = mp[1].second + mp[2].second;
                cout << "Files Count = " << totolFileCnt << endl;
                cout << "Max File Depth = " << max_fileDepth << endl; 
                
                auto empIt = fileExtensionMap.find(SC_EMPTY_SUFFIX);
                if ( empIt != fileExtensionMap.end() ) {
                    cout << SC_EMPTY_SUFFIX << " Suffix : " << endl;
                    size_t allSz = empIt->second.singleext_fileList.size();
                    int digitSz = getDigitCount(allSz);
                    for( size_t i = 0; i < allSz; ++i ) {
                        cout << "  " << setfill(' ') << setw(digitSz) << (i+1) << ". " << empIt->second.singleext_fileList[i].filename << endl;
                    }
                }

                auto lastDotIt = fileExtensionMap.find( SC_DOT_SUFFIX );
                if ( lastDotIt != fileExtensionMap.end() ) {
                    cout << SC_DOT_SUFFIX << " Suffix : " << endl;

                    size_t allSz = lastDotIt->second.singleext_fileList.size();
                    int digitSz = getDigitCount(allSz);
                    for( size_t i = 0; i < allSz; ++i ) {
                        cout << "  " << setfill(' ') << setw(digitSz) << (i+1) << ". "<< lastDotIt->second.singleext_fileList[i].filename << endl;
                    }

                    if ( !lastDotIt->second.multiext_fileList.empty() ) {
                        for ( auto iIt = lastDotIt->second.multiext_fileList.begin(); iIt != lastDotIt->second.multiext_fileList.end(); ++iIt ) {
                            size_t allSz = iIt->second.size();
                            int digitSz = getDigitCount(allSz);
                            cout << "    " << "|" << iIt->first  << "|" << endl;
                            for( size_t i = 0; i < allSz; ++i ) {
                                cout << "      " << setfill(' ') << setw(digitSz) << (i+1) << ". " << iIt->second[i].filename << endl;
                            }
                        }
                    }
                }


                for( auto it = fileExtensionMap.begin(); it != fileExtensionMap.end(); ++it ) {
                    if ( it->first != SC_EMPTY_SUFFIX && it->first != SC_DOT_SUFFIX ) {
                        cout << "\"" << it->first << "\"" << endl;
                        auto singleSz = it->second.singleext_fileList.size();
                        int digitSz = getDigitCount(singleSz);
                        if ( singleSz > 0 ) {
                            for ( size_t i = 0; i < singleSz; ++i ) {
                                cout << "    " << setfill(' ') << setw(digitSz) << (i+1) << ". " << it->second.singleext_fileList[i].filename << endl;
                            }
                        }

                        auto multiSz = it->second.multiext_fileList.size();
                        if ( multiSz > 0 ) {
                            for ( auto iIt = it->second.multiext_fileList.begin();  iIt != it->second.multiext_fileList.end(); ++iIt ) {
                                cout << "  " << "\"" << iIt->first << "\"" << endl;

                                auto mtSz = iIt->second.size();
                                int digitSz = getDigitCount(mtSz);
                                for ( size_t i = 0; i < mtSz; ++i ) {
                                    cout << "    " << setfill(' ') << setw(digitSz) << (i+1) << ". " << ((iIt->second)[i]).filename << endl;
                                }
                            }

                        }
                    }
                }

                cout << "===========================================================================" << endl;
            }
		} else if ( pr.second.second > 0 ) {
			cout << pr.second.first << " Count : " << pr.second.second << endl;
		}
	}
    // cout << "----------------------------------------" << endl;

    return 0;
}






