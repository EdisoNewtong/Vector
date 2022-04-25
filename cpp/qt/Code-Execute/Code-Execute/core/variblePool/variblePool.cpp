#include "variblePool.h"
#include "myException.h"
#include "enumUtil.h"
#include <iostream>
using namespace std;

#include <QTextStream>

// static
VariblePool* VariblePool::s_gPool = nullptr;

// static 
void VariblePool::init()
{
    if ( s_gPool != nullptr ) {
        return;
    }

    s_gPool = new VariblePool();
}

// static 
VariblePool* VariblePool::getPool()
{
    return s_gPool;
}

// static 
void VariblePool::finalize()
{
    INNER_SAFE_DELETE(s_gPool)

}


VariblePool::VariblePool()
    : m_pool()
    , m_varDefinedOrder()
{
    m_pool.clear();
    m_varDefinedOrder.clear();
}
    
// virtual 
VariblePool::~VariblePool()
{
    for( auto it = m_pool.begin(); it != m_pool.end(); ++it )
    {
        auto pVaribleInfo = it->second;
        if ( pVaribleInfo != nullptr ) {
            delete pVaribleInfo;
            it->second = nullptr;
        }
    }
    m_pool.clear();

    m_varDefinedOrder.clear();
}


VaribleInfo* VariblePool::getVaribleByName(const std::string& varname)
{
    auto foundIt = m_pool.find(varname);
    if ( foundIt == m_pool.end() ) {
        return nullptr;
    }

    return foundIt->second;
}
        



VaribleInfo* VariblePool::create_a_new_varible(E_DataType dt, const std::string& varname, int defline)
{
    auto foundIt = m_pool.find(varname);
    if ( foundIt != m_pool.end() ) {
        // already found it
        MyException e(E_THROW_VARIBLE_ALREADY_DEFINED);
        e.setDetail( varname + std::string(" @line : ") + std::to_string(defline) +  std::string(" has already been defined @line : ") + std::to_string( foundIt->second->definedLine) );
        throw e;
    }

    VaribleInfo* newVarible = new VaribleInfo();
    newVarible->dataVal.type = dt;
    newVarible->varName = varname;
    newVarible->isInitialed = false; // when create , set initial flag as false
    newVarible->definedLine = defline;

    m_pool.insert( make_pair(varname, newVarible) );
    m_varDefinedOrder.push_back( varname );

    return newVarible;
}



QString VariblePool::printAllVaribles(unsigned int flag)
{
    static const bool previousHasEqual = true;

    QString retstr;
    QTextStream ts(&retstr);

    if ( m_varDefinedOrder.empty() ) {
        ts << "<Empty> VariblePool" << endl;
        return retstr;
    }


    int idx = 0;
    for( const auto& varname : m_varDefinedOrder ) {
        auto it = m_pool.find(varname);
        if ( it != m_pool.end() ) {
            VaribleInfo* pVarInfo = it->second;
            if ( pVarInfo != nullptr ) {
                ts << (idx+1) << ".  " << EnumUtil::enumName(pVarInfo->dataVal.type).c_str() << " " << pVarInfo->varName.c_str() << " = " << pVarInfo->dataVal.getPrintValue(flag, previousHasEqual).c_str() << endl;
            }
        }

        ++idx;
    }

    return retstr;
}
