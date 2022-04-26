#ifndef VARIBLE_POOL_H
#define VARIBLE_POOL_H

#include "commonEnum.h"
#include "tokenBase.h"
#include "dataValue.h"
#include <string>
#include <unordered_map>

struct VaribleInfo
{
    std::string varName;
    DataValue dataVal;
    bool isInitialed;
};

class VariblePool
{
public: 
    static void init();
    static VariblePool* getPool();
    static void finalize();

public:
    VaribleInfo* create_a_new_varible(E_DataType dt, const std::string& varname);
    VaribleInfo* getVaribleByName(const std::string& varname);
protected:
    std::unordered_map<std::string, VaribleInfo*> m_pool;

private:
    VariblePool();
    virtual ~VariblePool();

    static VariblePool* s_gPool;


};


#endif
