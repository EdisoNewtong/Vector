#include "variblePool.h"
#include "myException.h"

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
{
    m_pool.clear();
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
}


VaribleInfo* VariblePool::getVaribleByName(const std::string& varname)
{
    auto foundIt = m_pool.find(varname);
    if ( foundIt == m_pool.end() ) {
        return nullptr;
    }

    return foundIt->second;
}
        



VaribleInfo* VariblePool::create_a_new_varible(E_DataType dt, const std::string& varname)
{
    auto foundIt = m_pool.find(varname);
    if ( foundIt != m_pool.end() ) {
        // already found it
        MyException e(E_THROW_VARIBLE_ALREADY_DEFINED);
        e.setDetail( varname );
        throw e;
    }

    VaribleInfo* newVarible = new VaribleInfo();
    newVarible->dataVal.type = dt;
    newVarible->varName = varname;
    newVarible->isInitialed = false; // when create , set initial flag as false

    m_pool.insert( make_pair(varname, newVarible) );

    return newVarible;
}

