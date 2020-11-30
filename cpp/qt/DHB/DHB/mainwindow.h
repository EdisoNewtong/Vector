#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
		enum E_UN_SIGNED
		{
			E_UNSIGNED = 0,
			E_SIGNED = 1,
		};

		enum E_DATA_TYPE
		{
			E_CHAR = 0,
			E_SHORT = 1,
			E_INT   = 2,
			E_LONG = 3,
		};

		// Hex / Binary
		enum E_BIN_HEX
		{
			E_HEX = 0,
			E_BIN = 1,
		};

		enum E_CHAR_NUMBER
		{
			E_CHAR_ONLY = 0,
			E_CHAR_NUMBER_ONLY = 1,
		};
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
        void on_pushButton_clicked();

		// unsinged / signed
        void on_radioButton_clicked();	  // unsigned
        void on_radioButton_2_clicked();  // signed

		// char / short / int / long
        void on_radioButton_3_clicked();	// char
        void on_radioButton_4_clicked();	// short
        void on_radioButton_5_clicked();	// int
        void on_radioButton_6_clicked();	// long

		void on_lineEdit_returnPressed();
        void on_checkBox_toggled(bool checked);

		// Binary / Hex / Decimal
        void on_radioButton_7_clicked();	// binary Mode
        void on_radioButton_8_clicked();	// Hex Mode
        void on_radioButton_9_clicked();	// Decimal Mode

		// input :   Char / Number ?
        void on_radioButton_10_clicked();	// input Char Only
        void on_radioButton_11_clicked();	// input ASCII Number

		// Copy to Result Clipboard
		void on_btnCopyHex_clicked();
		void on_btnCopyBin_clicked();
private:
    Ui::MainWindow *ui;

	E_UN_SIGNED m_signed_flag;
	E_DATA_TYPE m_dataType;
	E_CHAR_NUMBER m_char_number;

	QVector<QLabel*>		   m_HexVec;
	QVector<QLabel*>		   m_HexLineVec;
	QVector<QLabel*>		   m_BinVec;
	QVector<QLabel*>		   m_BinLineVec;
	QByteArray				   m_retHexBa;
	QByteArray				   m_retBinBa;
	QByteArray				   m_retDecBa;
	bool					   m_convertOK;
	int                        m_BaseMode;	// Input-Mode-Flag  :    Binary / Hex / Decial
	bool                       m_is1stUpdate;

	void destroyCreatedUI();
	void initUI();
	void initHexUI();
	void initBinUI();

	void updateUI();

	void reArrangeUI(bool isChanged);
	void reArrangeHexUI(bool isChanged);
	void reArrangeBinUI(bool isChanged);

	QString makeDesc();
	int calcNeededLabeCount(E_BIN_HEX tp);
	void updateHexVec();
	void updateBinVec();
	void setDecimalLabelVisible();
	void updateDecimalLabel();
	void updateDetailInfo();
	bool checkHexBinaryValid(const QString& content);
	QByteArray hex_bin_convert(const QString& content);

	// Debug Only
	int getVisibleLableCount(QVector<QLabel*>* vec);


	void doConvert(const QString& content);
	void doConvertUChar(const QString& content);
	void doConvertSChar(const QString& content);
	void doConvertUShort(const QString& content);
	void doConvertSShort(const QString& content);
	void doConvertUInt(const QString& content);
	void doConvertSInt(const QString& content);
	void doConvertULong(const QString& content);
	void doConvertSLong(const QString& content);

	static bool s_isShowUnunsedDigit;
	
	static const int s_STARTXPOS;

	static const int s_WIDTH_FOR_BIN;
	static const int s_HEIGHT;

	static const int s_SPACE1;
	static const int s_SPACE2;
	static const int s_SPACE3;



	// convert factor rate
	static const int sc_BYTE2HexBits;
	static const int sc_BYTE2BinBits;

	// unit in byte
	static const size_t sc_CHAR_SIZE;
	static const size_t sc_SHORT_SIZE;
	static const size_t sc_INT_SIZE;
	static const size_t sc_LONG_SIZE;
	static const size_t sc_LONGLONG_SIZE;

	// statu Bar ShowTime
	static const int  sc_StatuBarShowMsgTime;
};

#endif // MAINWINDOW_H
