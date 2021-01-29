#include <iostream>
#include "xlnt/xlnt.hpp"
using namespace std;

void testRead(const char* filename, int flag)
{
    std::string sfilenmae = filename;
    xlnt::path pathfilename(sfilenmae);
    xlnt::workbook wb;
    wb.load(pathfilename);
    
    cout << "load excel file <" << filename << ">   OK " << endl;

    auto sheetCnt = wb.sheet_count();
    cout << "sheetCnt = " << sheetCnt << endl;

    for ( int i = 0; i < sheetCnt; ++i ) {
        auto iSheet = wb.sheet_by_index(i);
        cout << (i+1) <<". id = " <<  iSheet.id() << ", title = " << iSheet.title() << endl; 
    }

    // row_t lowest_row() const;
    // row_t highest_row() const;
    // column_t lowest_column() const;
    // column_t highest_column() const;

    auto iSheet0 = wb.sheet_by_index(0);
    int rowIdx = 1;
    auto bflag = (flag == 1);
    cout << "bFlag" << (bflag ? " = true" : " = false") << endl;

    /*

        How To get the valid range of all the Cell ??

    //
    // method-1
    //
    cout << "lowest_row = " << iSheet0.lowest_row() << endl;
    cout << "highest_row = " << iSheet0.highest_row() << endl;
    auto low_col = iSheet0.lowest_column();
    auto high_col = iSheet0.highest_column();

    cout << "lowest_column = "  << low_col.index  << " = " << low_col.column_string() << endl;
    cout << "highest_column = " << high_col.index << " = " << high_col.column_string() << endl;
    // cout << "lowest_column = " << (int)iSheet0.lowest_column() << endl;
    // cout << "highest_column = " << (int)iSheet0.highest_column() << endl;


    //
    // method-2
    //
    auto rangeref = iSheet0.calculate_dimension();
    cout << "rangeref.width = " << rangeref.width() << endl;
    cout << "rangeref.height = " << rangeref.height() << endl;



    return;
    */


    for ( auto row : iSheet0.rows(bflag) ) {
        int cellIdx = 1;
        for( auto cell : row ) {
            std::string content = cell.to_string();
            cout << "[ " << rowIdx << "," << cellIdx << " ]   ==> Content : \"" <<  content << "\"" << (content.empty() ? " => Zero" : "") << endl;
            ++cellIdx;
        }

        cout << "\tcolSize = " << (cellIdx - 1) << endl;
        ++rowIdx;
    }

    cout << "rowSize = " << (rowIdx - 1) << endl;
}

int main(int argc,char* argv[])
{
    if ( argc != 3) {
        cout << "argc != 2 , return " << endl;
        return -1;
    }

    try {
        testRead(argv[1], atoi(argv[2]));
    } catch ( std::exception& e ) {
        cout << "Meet error : error is [ " << e.what() << " ]" << endl;
    }

    return 0;
}


