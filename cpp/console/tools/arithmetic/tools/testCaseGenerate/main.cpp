#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
using namespace std;

const string G_Extention = ".txt";

struct DetailInfo
{
    string fileNameLabel;
    string fileContent;
    DetailInfo(const string& label, const string& content) : fileNameLabel(label),  fileContent(content) { }
};

DetailInfo G_CfgTable[] = {
    DetailInfo("Nullptr", "" ), // 0
    DetailInfo("Semi", ";" ),   // 1

    // Sequence   [2,8] : 7 types
    DetailInfo("Decimal", "123" ),   // 2
    DetailInfo("Hex", "0x1F" ),   
    DetailInfo("Oct", "077" ),   
    DetailInfo("Float", "3.14f" ),   
    DetailInfo("Double", "1.414" ),   
    DetailInfo("Varible", "var" ),   
    DetailInfo("Int", "int" ),     // 8
                                   
    // Operator   [9,32]  : 24 types
    DetailInfo("Op_Add", "+" ),      // 9
    DetailInfo("Op_Minus", "-" ),      
    DetailInfo("Op_Multiply", "*" ),  
    DetailInfo("Op_Divide", "/" ),   
    DetailInfo("Op_Mod", "%" ),      
    DetailInfo("Op_BitAnd", "&" ),
    DetailInfo("Op_BitOr", "|" ),
    DetailInfo("Op_BitXOr", "^" ),
    DetailInfo("Op_BitNot", "~" ),
    DetailInfo("Op_BitLeftShift", "<<"),
    DetailInfo("Op_BitRightShift", ">>"),
    DetailInfo("Op_OpenParentheses", "("),    // 20
    DetailInfo("Op_CloseParentheses", ")"),
    DetailInfo("Op_Assignment", "="),
    DetailInfo("Op_Add_Assignment", "+="),
    DetailInfo("Op_Minus_Assignment", "-="),
    DetailInfo("Op_Multiply_Assignment", "*="),
    DetailInfo("Op_Divide_Assignment", "/="),
    DetailInfo("Op_Mod_Assignment", "%="),
    DetailInfo("Op_BitAnd_Assignment", "&="),
    DetailInfo("Op_BitOr_Assignment", "|="),
    DetailInfo("Op_BitXOr_Assignment", "^="),
    DetailInfo("Op_BitLeftShift_Assignment", "<<="),
    DetailInfo("Op_BitRightShift_Assignment", ">>=")

};






int G_checkCnt = 0;


int main(int argc, char* argv[])
{
    const int SZSZ = sizeof(G_CfgTable) / sizeof(G_CfgTable[0]);
    cout << "Size = " << SZSZ << endl;

    int CreatefileCnt = 0;
    for( int i = 0; i < SZSZ; ++i ) {
        for( int j = 1; j < SZSZ; ++j ) {
            string dirPath;
            int tagtag = 0;
            if ( j == 1 ) {
                dirPath = "Semi/";
                tagtag = 1;
            } else if ( j >= 2 && j<=8 ) {
                dirPath = "Seq/";
                tagtag = 2;
            } else if ( j >= 9 ) { // j == 3     
                dirPath = "Op/";
                tagtag = 3;
            }

            string fullFileName = dirPath + G_CfgTable[i].fileNameLabel + "-" + G_CfgTable[j].fileNameLabel + G_Extention;
            if ( tagtag == 3 ) {
                // cout << "Create File [1] : " << fullFileName << endl;
                cout << fullFileName << endl;
                ++G_checkCnt;
            }

            if ( (j == 1 || j == 8) && tagtag == 2 ) {
                // cout << endl;
            }

            ifstream existedfile(fullFileName.c_str(), ios::in);
            if ( existedfile ) {
                existedfile.close();
                cout << "[ERROR] : file \"" << fullFileName << "\" is already existed." << endl;
            } else {
                existedfile.close();

                ++CreatefileCnt;

                ofstream outFile( fullFileName.c_str(), ios::out | ios::trunc);
                string   content; 
                
                if ( (i>=2 && i<=8)  && (j>=2 && j<=8) ) {
                    content = (G_CfgTable[i].fileContent + " " + G_CfgTable[j].fileContent);
                } else {
                    if ( tagtag == 3 && i>=9 ) {
                        if ( i != 20 ) {
                            //                                                                 No <Space> in the middle
                            content = string("int var = 5;\nvar") + (G_CfgTable[i].fileContent + G_CfgTable[j].fileContent);
                        } else {
                            //                                                        No <Space> in the middle
                            content = string("int var = 5;\n") + (G_CfgTable[i].fileContent + G_CfgTable[j].fileContent);
                        }
                    } else {
                        content = (G_CfgTable[i].fileContent + G_CfgTable[j].fileContent);
                    }
                }

                outFile << content;
                outFile.flush();
                outFile.close();


                if ( i>=9 && j>=9 ) {
                    ++CreatefileCnt;

                    string fullFileName2 = dirPath + G_CfgTable[i].fileNameLabel + "-" + G_CfgTable[j].fileNameLabel + "-2" + G_Extention;
                    if ( tagtag == 3 ) {
                        // cout << fullFileName2 << endl;
                    }

                    ofstream outFile2(fullFileName2.c_str(), ios::out | ios::trunc);
                    string   content2; 
                    if ( i != 20 ) {
                        //                                                                 One <Space> in the middle
                        content2 = string("int var1 = 5;\nvar1") + (G_CfgTable[i].fileContent + " " + G_CfgTable[j].fileContent);
                    } else {
                        //                                                                 One <Space> in the middle
                        content2 = string("int var1 = 5;\n") + (G_CfgTable[i].fileContent + " " + G_CfgTable[j].fileContent);
                    }

                    outFile2 << content2;
                    outFile2.flush();
                    outFile2.close();
                }


                if ( j == (SZSZ-1) && tagtag == 3 ) {
                     // cout << endl;
                }
            }
        }

        
    }

    cout << "Totally create file count : " << CreatefileCnt << endl;
    cout << "G_checkCnt = " << G_checkCnt << endl;

    return 0;
}