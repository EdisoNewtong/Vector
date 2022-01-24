#include <QFile>

#include "encryptdecrypt.h"




// static 
char ende::enc1(const char& byte)
{
	return ~byte;
}

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
		// swap if
		if( bit4th != 0) {
			ret = static_cast<char>( ui_code | (1U<<3U)   &   (~(1U<<4U)) );
		} else {
			// bit5th != 0
			ret = static_cast<char>( ui_code | (1U<<4U)   &   (~(1U<<3U)) );
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

// static 
char ende::enc4(const char& byte)
{
	return byte;
}



// static 
QByteArray ende::encrypt(const QString& fileabspath)
{
	QByteArray ret;
	
	int osType = -1;
#ifdef Q_OS_WIN
	osType = static_cast<int>(ende::E_WINDOWS);
#elif defined(Q_OS_MAC)
	osType = static_cast<int>(ende::E_MAC);
#elif defined(Q_OS_LINUX)
	osType = static_cast<int>(ende::E_LINUX);
#else
	osType = static_cast<int>(-1);
#endif


	QFile file(fileabspath);
	if( file.open( QIODevice::ReadOnly ) ) {

	}

	return ret;
}



// static 
QString ende::getOSTypeById(int osTag)
{
	QString strOSType;
	switch(osTag)
	{
	case ende::E_WINDOWS:
		strOSType = "Windows";
		break;
	case ende::E_MAC:
		strOSType = "Mac";
		break;
	case ende::E_LINUX:
		strOSType = "Linux";
		break;
	default:
		strOSType = "Unknown";
		break;
	}

	return strOSType;
}



