#include "util.h"

Util::Util(int times) : m_times(times)
{
}



Util::~Util()
{
}


int Util::calculateSum(int beg, int end) const
{
    auto sumRet = 0;
    for( auto i = beg; i <=end; ++i ) {
        sumRet += i;
    }
    return sumRet * m_times;
}



