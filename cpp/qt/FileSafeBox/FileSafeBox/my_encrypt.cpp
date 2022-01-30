#include "my_encrypt.h"

//
// static member function
//

/*
// static 
char ende::enc2(const char& byte)
{
	char ret = byte;
	unsigned int ui_code = (byte & 0xFF);
	//    4<->5	, swap the 4th bit and the 5th bit
    // 1234 | 5678
	// XXXX | XXXX
	unsigned int bit4th = ((ui_code >> 4U)  & 0x1U);
	unsigned int bit5th = ((ui_code >> 3U)  & 0x1U);
	if( bit4th ^ bit5th ) {
        //////////////////////////////////////////////////////////////////
        //
        // Core Core Core
        // ************************************************
        //    the priority of '&'  is higher than  '|'
        // ************************************************
        //  
        // e.g.
        //          unsigned int num = 0x10;
        //          unsigned int result = num |  0x8 & 0x3;   
        //   // =>  unsigned int result = num | (0x8 & 0x3);
        //////////////////////////////////////////////////////////////////
		// swap if
		if( bit4th != 0) {

			ret = static_cast<char>( ( ui_code | (1 << 3) )     &   (~(1 << 4)) );
		} else {
			// bit5th != 0
			ret = static_cast<char>( ( ui_code | (1 << 4) )     &   (~(1 <<3)) );
		}
	} 

	// else  do nothing
	return ret;
	
}

// static 
char ende::enc3(const char& byte)
{
	// swap high 4bits <--> low 4bits
	unsigned int high4bits = (byte & 0xF0);
	unsigned int low4bits  = (byte & 0x0F);
	return static_cast<char>( (high4bits >> 4U) | (low4bits << 4U) );
}
*/




// static 
char encryptUtil::enc1(char originChar)
{
    return originChar;
}

// static 
char encryptUtil::enc2(char originChar)
{
    return originChar;
}


// static 
char encryptUtil::enc3(char originChar)
{
    return originChar;
}

// static 
char encryptUtil::enc4(char originChar)
{
    return originChar;
}



