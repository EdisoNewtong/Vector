#include "variblePool.h"
#include "keywordList.h"
#include "myException.h"

using namespace std;

//
// static Member Data Init
//
unordered_map<string,VaribleData*>  VariblePool::s_variblePool{};

// static
void VariblePool::init()
{
    s_variblePool.clear();
}


// static
void VariblePool::finalize()
{
    for( auto it = s_variblePool.begin(); it != s_variblePool.end(); ++it )
    {
        // VaribleData* pData = it->second;
        if ( it->second != nullptr ) {
            delete it->second;
            it->second = nullptr;
        }
    }
    s_variblePool.clear();
}


// static 
void VariblePool::insertVarible(E_DataType dt,const string& varName, const string& varValue)
{
    auto bIsKeyWord = KeyWordList::isKeyWord(varName);
    if ( bIsKeyWord ) {
        MyException e(E_VARIBLE_NAME_CANNOT_BE_KEYWORD);
        throw e;
    }


    auto foundIt = s_variblePool.find(varName);
    if ( foundIt != s_variblePool.end() ) {
        // found
        MyException e(E_VARIBLE_NAME_ALREADY_DECLARED);
        string detailstr(" '");
        detailstr += varName;
        detailstr += "' has been already declared previously ! ";
        e.setDetail( detailstr );
        throw e;
    }

    // else Not Found , build One New Varible and fill its value
    VaribleData* pData = new VaribleData;
    pData->name = varName;
    pData->dt = dt;

    // fill data
    switch( pData->dt  )
    {
    case E_TP_CHAR:
        pData->val.ch_val = static_cast<char>( stoi( varValue ) );
        break;
	case E_TP_U_CHAR:
        pData->val.un_ch_val = static_cast<unsigned char>( stoi( varValue ) );
        break;
	case E_TP_S_CHAR:
        pData->val.si_ch_val = static_cast<  signed char>( stoi( varValue ) );
        break;
	case E_TP_U_SHORT:
        pData->val.un_short_val = static_cast<unsigned short>( stoi( varValue ) );
        break;
	case E_TP_S_SHORT:
        pData->val.si_short_val = static_cast<  signed short>( stoi( varValue ) );
        break;
	case E_TP_U_INT:
        pData->val.un_int_val = static_cast<unsigned int>( stoi( varValue ) );
        break;
	case E_TP_S_INT:
        pData->val.si_int_val = static_cast<  signed int>( stoi( varValue ) );
        break;
	case E_TP_U_LONG:
        pData->val.un_long_val = static_cast<unsigned long>( stol( varValue ) );
        break;
	case E_TP_S_LONG:
        pData->val.si_long_val = static_cast<  signed long>( stol( varValue ) );
        break;
	case E_TP_U_LONG_LONG:
        pData->val.un_longlong_val = static_cast<unsigned long long>( stoll( varValue ) );
        break;
	case E_TP_S_LONG_LONG:
        pData->val.si_longlong_val = static_cast<  signed long long>( stoll( varValue ) );
        break;
	case E_TP_FLOAT:
        pData->val.float_val = stof( varValue );
        break;
	case E_TP_DOUBLE:
        pData->val.double_val = stod( varValue );
        break;
	case E_TP_LONG_DOUBLE:
        pData->val.longdouble_val = stold( varValue );
        break;
    default:
        MyException e(E_SWITCH_CASE_DEFAULT_FLOW);
        throw e;
        break;
    }

    // Success insert
    s_variblePool.insert( make_pair(varName, pData) );

}


