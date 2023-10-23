#ifndef VARIBLE_POOL_H
#define VARIBLE_POOL_H

#include "commonEnum.h"
#include "tokenBase.h"
#include "dataValue.h"
#include <string>
#include <unordered_map>
#include <vector>

#include <QString>

struct VaribleInfo
{
    std::string varName;
    DataValue dataVal;
    bool isInitialed;
    int  definedLine;

    VaribleInfo() 
        : varName()
        , isInitialed(false)
        , definedLine(0) 
    {
    }
};

class VariblePool
{
public: 
    static void init();
    static VariblePool* getPool();
    static void finalize();

public:
    VaribleInfo* create_a_new_varible(E_DataType dt, const std::string& varname, int defline);
    VaribleInfo* getVaribleByName(const std::string& varname);
    void         randomVaribleValue(const std::string& varname);

    QString printAllVaribles(unsigned int flag);
protected:
    std::unordered_map<std::string, VaribleInfo*> m_pool;
    std::vector<std::string>                      m_varDefinedOrder;

private:
    VariblePool();
    virtual ~VariblePool();

    static VariblePool* s_gPool;


};


#endif