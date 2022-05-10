#include "EncdecUtil.h"



char EncryptDecryptUtil::enc1ch(int type, char ch)
{
    char retCh = 0;
    if ( type == 1 ) {
        retCh = enc1(ch);
    } else if ( type == 2 ) {
        retCh = enc2(ch);
    } else if ( type == 3 ) {
        retCh = enc3(ch);
    } else if ( type == 4 ) {
        retCh = enc4(ch);
    } else {
        retCh = ch;
    }

    return retCh;
}

char EncryptDecryptUtil::dec1ch(int type, char ch)
{
    char retCh = 0;
    if ( type == 1 ) {
        retCh = dec1(ch);
    } else if ( type == 2 ) {
        retCh = dec2(ch);
    } else if ( type == 3 ) {
        retCh = dec3(ch);
    } else if ( type == 4 ) {
        retCh = dec4(ch);
    } else {
        retCh = ch;
    }

    return retCh;
}






bool EncryptDecryptUtil::enc2ch(int type, char ch1, char ch2, char* pOut1, char* pOut2)
{
    auto bret = true;
    if ( type == 1 ) {
        encWord1(ch1,ch2, pOut1, pOut2);
    } else if ( type == 2 ) {
        encWord2(ch1,ch2, pOut1, pOut2);
    } else if ( type == 3 ) {
        encWord3(ch1,ch2, pOut1, pOut2);
    } else if ( type == 4 ) {
        encWord4(ch1,ch2, pOut1, pOut2);
    } else {
        bret = false;
    }

    return bret;
}


bool EncryptDecryptUtil::dec2ch(int type, char ch1, char ch2, char* pOut1, char* pOut2)
{
    auto bret = true;
    if ( type == 1 ) {
        decWord1(ch1,ch2, pOut1, pOut2);
    } else if ( type == 2 ) {
        decWord2(ch1,ch2, pOut1, pOut2);
    } else if ( type == 3 ) {
        decWord3(ch1,ch2, pOut1, pOut2);
    } else if ( type == 4 ) {
        decWord4(ch1,ch2, pOut1, pOut2);
    } else {
        bret = false;
    }

    return bret;
}



char EncryptDecryptUtil::enc1(char ch)
{
    return ~ch;
}

char EncryptDecryptUtil::enc2(char ch)
{
    // swap lower 4 bits and higher 4 bits
    //             original :   8765 4321
    //      after transform :   4321 8765
    return  static_cast<char>(   ( (ch >> 4) & 0x0F )     // higer 4 bit    >> 4
                               | ( (ch << 4) & 0xF0 ) );  // lower 4 bit    << 4
}

char EncryptDecryptUtil::enc3(char ch)
{
    // reverse bits
    //             original :   8765 4321
    //      after transform :   1234 5678
    return    static_cast<char>(   ( (ch >> 7) & 0x01 )
                                 | ( (ch >> 5) & 0x02 )
                                 | ( (ch >> 3) & 0x04 )
                                 | ( (ch >> 1) & 0x08 )
                                 | ( (ch << 1) & 0x10 )
                                 | ( (ch << 3) & 0x20 )
                                 | ( (ch << 5) & 0x40 )
                                 | ( (ch << 7) & 0x80 ) );
}

char EncryptDecryptUtil::enc4(char ch)
{
    //  left shift 1 bit           
    //             original :   8765 4321
    //      after transform :   7654 3218
    return  static_cast<char>(     (ch << 1)
                               | ( (ch >> 7) & 0x01) );

}

char EncryptDecryptUtil::dec1(char ch)
{
    return ~ch;
}

char EncryptDecryptUtil::dec2(char ch)
{
    // swap lower 4 bits and higher 4 bits
    //      original :   8765 4321
    //           ret :   4321 8765
    return  static_cast<char>(   ( (ch >> 4) & 0x0F )    // higer 4 bit    >> 4
                               | ( (ch << 4) & 0xF0 ) ); // lower 4 bit    << 4
}

char EncryptDecryptUtil::dec3(char ch)
{
    // swap 8 <-> 1
    //      7 <-> 2
    //      6 <-> 3
    //      5 <-> 4
    
    //             original :   8765 4321
    //      after transform :   4321 8765
    return static_cast<char>(   ( (ch >> 7) & 0x01 )
                              | ( (ch >> 5) & 0x02 )
                              | ( (ch >> 3) & 0x04 )
                              | ( (ch >> 1) & 0x08 )
                              | ( (ch << 1) & 0x10 )
                              | ( (ch << 3) & 0x20 )
                              | ( (ch << 5) & 0x40 )
                              | ( (ch << 7) & 0x80 ) );

}

char EncryptDecryptUtil::dec4(char ch)
{
    //  restore left shift 1 bit  ( Do Right Bit Shift )         
    //             original :   8765 4321
    //      after transform :   7654 3218
    return static_cast<char>(   ( (ch >> 1) & 0x7F)
                              | ( (ch << 7) & 0x80) );
}














//==================================================================================================
//==================================================================================================
//==================================================================================================
//==================================================================================================
//==================================================================================================









void EncryptDecryptUtil::encWord1(char ch1, char ch2, char* pOut1, char* pOut2)
{
    // swap ( the 1st byte's lower 4 bits ) and ( the 2nd byte's higher 4 bits )
    if ( pOut1 != nullptr ) {
        *pOut1 = static_cast<char>(      ( ch1 & 0xF0 ) 
                                    |  ( ( ch2 >>   4 ) & 0x0F) );
    }

    if ( pOut2 != nullptr ) {
        *pOut2 = static_cast<char>(   ( ch1 << 4 )
                                    | ( ch2 & 0x0F) );
    }
}

void EncryptDecryptUtil::encWord2(char ch1, char ch2, char* pOut1, char* pOut2)
{
    // swap the last bit of ch1 and the leader bit of ch2
    if ( pOut1 != nullptr ) {
        *pOut1 = static_cast<char>(   ( ch1 & 0xFE )
                                    | ( ( ch2 >>   7 ) & 0x1 ) );
    } 

    if ( pOut2 != nullptr ) {
        *pOut2 = static_cast<char>(     ( ch2 & 0x7F )
                                    | ( ( ch1 & 0x1) << 7 ) );
    } 

}

void EncryptDecryptUtil::encWord3(char ch1, char ch2, char* pOut1, char* pOut2)
{
    // <=  left shift 1 bit of 2 chars , make the 2nd char the last bit as the leader bit of ch1
    if ( pOut1 != nullptr ) {
        *pOut1 = static_cast<char>(     ( ch1 << 1)
                                    | ( ( ch2 >> 7) & 0x1) );
    }

    if ( pOut2 != nullptr ) {
        *pOut2 = static_cast<char>(     ( ch2 << 1)
                                    | ( ( ch1 >> 7) & 0x1) );
    }
}

void EncryptDecryptUtil::encWord4(char ch1, char ch2, char* pOut1, char* pOut2)
{
    // swap ( the 1st byte's leader bit ) and ( the 2nd byte's lower bit )
    if ( pOut1 != nullptr ) {
        *pOut1 = static_cast<char>(   ( ch1 & 0x7F ) 
                                    | ( ch2 <<   7 ) );
    }

    if ( pOut2 != nullptr ) {
        *pOut2 = static_cast<char>(     ( ch2 & 0xFE ) 
                                    | ( ( ch1 >>  7  ) & 0x1 ) );
    }
}

void EncryptDecryptUtil::decWord1(char ch1, char ch2, char* pOut1, char* pOut2)
{
    // swap ( the 1st byte's lower 4 bits ) and ( the 2nd byte's higher 4 bits )
    if ( pOut1 != nullptr ) {
        *pOut1 = static_cast<char>(     ( ch1 & 0xF0 ) 
                                    | ( ( ch2 >>   4 ) & 0x0F) );
    }

    if ( pOut2 != nullptr ) {
        *pOut2 = static_cast<char>(   ( ch1 << 4 )
                                    | ( ch2 & 0x0F) );
    }
}

void EncryptDecryptUtil::decWord2(char ch1, char ch2, char* pOut1, char* pOut2)
{
    // swap the last bit of ch1 and the leader bit of ch2
    if ( pOut1 != nullptr ) {
        *pOut1 = static_cast<char>(     ( ch1 & 0xFE )
                                    | ( ( ch2 >>   7 ) & 0x1 ) );
    } 

    if ( pOut2 != nullptr ) {
        *pOut2 = static_cast<char>(     ( ch2 & 0x7F )
                                    | ( ( ch1 & 0x1) << 7 ) );
    } 
}

void EncryptDecryptUtil::decWord3(char ch1, char ch2, char* pOut1, char* pOut2)
{
    // => right shift 1 bit of 2 chars , make the 2nd char the last bit as the leader bit of ch1
    if ( pOut1 != nullptr ) {
        *pOut1 = static_cast<char>(   (  ( ch1 >> 1) & 0x7F )
                                    | (  ( ch2 << 7) & 0x80 ) );
    }

    if ( pOut2 != nullptr ) {
        *pOut2 = static_cast<char>(   ( ( ch2 >> 1) & 0x7F )
                                    | ( ( ch1 << 7) & 0x80 ) );
    }
}

void EncryptDecryptUtil::decWord4(char ch1, char ch2, char* pOut1, char* pOut2)
{
    // swap ( the 1st byte's leader bit ) and ( the 2nd byte's lower bit )
    if ( pOut1 != nullptr ) {
        *pOut1 = static_cast<char>(    ( ch1 & 0x7F ) 
                                     | ( ch2 <<   7 ) );
    }

    if ( pOut2 != nullptr ) {
        *pOut2 = static_cast<char>(     ( ch2 & 0xFE ) 
                                    | ( ( ch1 >>  7  )   & 0x1 ) );
    }
}



