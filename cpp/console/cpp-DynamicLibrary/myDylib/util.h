#pragma once

class Util
{
public:
    Util(int times);
    virtual ~Util();

    int calculateSum(int beg,int end) const;
protected:
    int m_times;
};


