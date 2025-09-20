
local BaseDataTypes = {
    bool   = true,

    uchar  = true,
    schar  = true,
     char  = true,

    u_short = true,
      short = true,

    u_int = true,
      int = true,

    u_long = true,
      long = true,

    u_longlong = true,
      longlong = true,

    float = true,
    double = true,

    string = true, -- alias ==> std::string  ( #include <string>  using namespace std; )
}

return BaseDataTypes;


