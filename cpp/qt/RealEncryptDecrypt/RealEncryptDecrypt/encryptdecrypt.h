#ifndef    ENCRYPT_DECRYPT_H
#define    ENCRYPT_DECRYPT_H


#include <QByteArray>
#include <QString>

class ende
{
public:
	static QByteArray encrypt(const QString& fileabspath);
	static QByteArray decrypt(const QString& fileabspath);

protected:
	enum OS_TYPE {
		E_WINDOWS = 0,
		E_MAC,
		E_LINUX,
	};

	static char enc1(const char& byte);
	static char enc2(const char& byte);
	static char enc3(const char& byte);
	static char enc4(const char& byte);

	static QString getOSTypeById(int osTag);
};

#endif
