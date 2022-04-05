#ifndef VARIBLE_POOL_H
#define VARIBLE_POOL_H

#include "commonEnum.h"
#include "varibleValue.h"

#include <unordered_map>
#include <string>


class VariblePool
{
public:
	static void init();
	static void finalize();

    static void insertVarible(E_DataType dt,const std::string& varName, const std::string& varValue);
protected:
    static std::unordered_map<std::string,VaribleData*> s_variblePool;

};

#endif
