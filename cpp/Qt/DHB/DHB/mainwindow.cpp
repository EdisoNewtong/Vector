#include <QtDebug>
#include <QTextStream>
#include <QToolTip>
#include <QClipboard>
#include <QPoint>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <sstream>
#include <iostream>
#include <string>
#include <limits>
using namespace std;

// static
bool MainWindow::s_isShowUnunsedDigit = true;

// static
const int MainWindow::s_STARTXPOS = 110;
// static 
const int MainWindow::s_WIDTH_FOR_BIN = 10;
// static 
const int MainWindow::s_HEIGHT = 40;

// static 
const int MainWindow::s_SPACE1 = 0;
// static 
const int MainWindow::s_SPACE2 = 7;
// static 
const int MainWindow::s_SPACE3 = 10;




// static 
const size_t MainWindow::sc_CHAR_SIZE = sizeof(char);
// static 
const size_t MainWindow::sc_SHORT_SIZE = sizeof(short);
// static 
const size_t MainWindow::sc_INT_SIZE = sizeof(int);
// static 
const size_t MainWindow::sc_LONG_SIZE = sizeof(long);
// static 
const size_t MainWindow::sc_LONGLONG_SIZE = sizeof(long long);

// static 
const int MainWindow::sc_BYTE2HexBits = 2;
const int MainWindow::sc_BYTE2BinBits = 8;


// static
const int MainWindow::sc_StatuBarShowMsgTime = 3000;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::MainWindow)
	// m_testLabel(nullptr),
	, m_signed_flag(E_SIGNED)
	, m_dataType(E_INT)
	, m_char_number(E_CHAR_ONLY)
	, m_convertOK(false)
	, m_BaseMode(10) // Decimal Mode
	, m_is1stUpdate(true)
{
	m_retHexBa.clear();
	m_retBinBa.clear();
	m_retDecBa.clear();

    ui->setupUi(this);

	initUI();

    setFixedSize( size() );
}


MainWindow::~MainWindow()
{
	destroyCreatedUI();
    delete ui;
}


void MainWindow::initUI()
{
    // this->setSizePolicy( QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, QSizePolicy::Frame) );
	if( sc_INT_SIZE == sc_LONG_SIZE ) {
		// need use long long to replace   long
		ui->radioButton_5->setText("int (<long>)");
		ui->radioButton_6->setText("long long");
	}

	ui->groupBox_3->setEnabled(true);
    ui->pushButton->hide();
	// ui->lineEdit_min->hide();
	// ui->lineEdit_max->hide();
	ui->label_5->hide();

	initHexUI();
	initBinUI();
    ui->checkBox->setChecked(s_isShowUnunsedDigit);
	ui->label_6->setVisible(m_BaseMode!=10);
	ui->groupBox_4->setVisible(m_dataType == E_CHAR);
	updateDetailInfo();

    ui->lineEdit->setPlaceholderText("Input a Decial number(10)");

	m_is1stUpdate = false;

}

void MainWindow::initHexUI()
{
	static const int WIDTH = s_WIDTH_FOR_BIN*4 + 3*s_SPACE1;
	static const int posY = 210;
	int num = calcNeededLabeCount(E_HEX);

	Q_ASSERT_X(m_HexVec.empty(),"MainWindow::initHexUI()","Hex Vector must be empty");
	Q_ASSERT_X(m_HexLineVec.empty(),"MainWindow::initHexUI()","Hex Line Vector must be empty");

	int calc_X = 0;
	for(int i = 0; i < num; ++i) {
		QLabel* create_label = new QLabel(ui->centralWidget);
		if( i == 0) {
			calc_X = s_STARTXPOS;
		} else {
			int space = (i%2==1 ? s_SPACE2 : s_SPACE3);
			calc_X += (space + WIDTH);
		}

		if(i>0 && (i+1)%2==0 ) {
			QLabel* line_Label = new QLabel(ui->centralWidget);
			line_Label->setGeometry( QRect(calc_X+WIDTH+2,posY,5, s_HEIGHT) );
			line_Label->setText("|");
			line_Label->setVisible(true);
			m_HexLineVec.push_back(line_Label);
		}

		// set font 
		QFont f_new( create_label->font() );
		f_new.setPointSize( f_new.pointSize() * 4);
		create_label->setFont(f_new);

		create_label->setGeometry( QRect(calc_X ,posY ,WIDTH,s_HEIGHT) );
		create_label->setText("-");
		create_label->setVisible(true);

		m_HexVec.push_back(create_label);
	}
}

void MainWindow::initBinUI()
{
	static const int posY = 260;
	int num = calcNeededLabeCount(E_BIN);
	// qDebug() << "BinVec num = " << num;

	Q_ASSERT_X(m_BinVec.empty(),"MainWindow::initBinUI()","Bin Vector must be empty");
	Q_ASSERT_X(m_BinLineVec.empty(),"MainWindow::initBinUI()","Bin Line Vector must be empty");

	int calc_X = 0;
	for(int i = 0; i < num; ++i) {
		QLabel* create_label = new QLabel(ui->centralWidget);
		if( i == 0) {
			calc_X = s_STARTXPOS;
		} else {
			int space = 0;//(i%4!=0 ? s_SPACE1 : s_SPACE);
			if( i%8 == 0 ) {
				space = s_SPACE3;
			} else {
				// i%8!=0
				if( i%4 == 0 ) {
					space = s_SPACE2;
				} else {
					space = s_SPACE1;
				}
			}
			calc_X += (space + s_WIDTH_FOR_BIN);
		}


		if(i>0 && (i+1)%8==0 ) {
			QLabel* line_Label = new QLabel(ui->centralWidget);
			line_Label->setGeometry( QRect(calc_X+ s_WIDTH_FOR_BIN +2,posY,5, s_HEIGHT) );
			line_Label->setVisible(true);
			line_Label->setText("|");
			m_BinLineVec.push_back(line_Label);
		}

		create_label->setGeometry( QRect(calc_X ,posY ,s_WIDTH_FOR_BIN,s_HEIGHT) );
		create_label->setVisible(true);
		create_label->setText("-");
		m_BinVec.push_back(create_label);
	}
}

void MainWindow::destroyCreatedUI()
{
	//
	// clear Hex Vector
	//
	for(auto it = m_HexVec.begin(); it!=m_HexVec.end(); ++it) {
		delete (*it);
		(*it) = nullptr;
	}
	m_HexVec.clear();

	for(auto it = m_HexLineVec.begin(); it!=m_HexLineVec.end(); ++it) {
		delete (*it);
		(*it) = nullptr;
	}
	m_HexLineVec.clear();


	//
	// clear Bin Vector
	//
	for(auto it = m_BinVec.begin(); it!=m_BinVec.end(); ++it) {
		delete (*it);
		(*it) = nullptr;
	}
	m_BinVec.clear();
	
	for(auto it = m_BinLineVec.begin(); it!=m_BinLineVec.end(); ++it) {
		delete (*it);
		(*it) = nullptr;
	}
	m_BinLineVec.clear();
}


void MainWindow::updateUI()
{
	updateHexVec();
	updateBinVec();
	updateDetailInfo();
}

int MainWindow::calcNeededLabeCount(E_BIN_HEX tp)
{
	int retNum = 0;
	if( tp == E_HEX ) {
		switch(m_dataType) 
		{
		case E_DATA_TYPE::E_CHAR:
			retNum = sc_CHAR_SIZE * sc_BYTE2HexBits;
			break;
		case E_DATA_TYPE::E_SHORT:
			retNum = sc_SHORT_SIZE * sc_BYTE2HexBits;
			break;
		case E_DATA_TYPE::E_INT:
			retNum = sc_INT_SIZE * sc_BYTE2HexBits;
			break;
		case E_DATA_TYPE::E_LONG:
			retNum = (sc_INT_SIZE==sc_LONG_SIZE ?  sc_LONGLONG_SIZE : sc_LONG_SIZE) * sc_BYTE2HexBits;
			break;
		default:
			break;
		}

	} else {
		// tp == E_BIN
		switch(m_dataType) 
		{
		case E_DATA_TYPE::E_CHAR:
			retNum = sc_CHAR_SIZE * sc_BYTE2BinBits;
			break;
		case E_DATA_TYPE::E_SHORT:
			retNum = sc_SHORT_SIZE * sc_BYTE2BinBits;
			break;
		case E_DATA_TYPE::E_INT:
			retNum = sc_INT_SIZE * sc_BYTE2BinBits;
			break;
		case E_DATA_TYPE::E_LONG:
			retNum = (sc_INT_SIZE==sc_LONG_SIZE ?  sc_LONGLONG_SIZE : sc_LONG_SIZE) * sc_BYTE2BinBits;
			break;
		default:
			break;
		}
	}

	return retNum;
}


void MainWindow::updateHexVec()
{
	int curSize = calcNeededLabeCount(E_HEX);
	//
	// update Label
	//
	if( m_retHexBa.isEmpty() ) {
		for(int i = curSize-1; i>=0; --i)
		{
			auto label = m_HexVec.at(i);
			if( !m_is1stUpdate ) {
				label->setText("");
			}
		}
	} else {
		//
		// s_isShowUnunsedDigit
		//
		int hex_idx = m_retHexBa.size()-1;

		for(int i = curSize-1; i>=0; --i)
		{
			QLabel* lb = m_HexVec.at(i);
			Q_ASSERT_X(lb!=nullptr && lb->isVisible(), "MainWindow::updateHexVec()", "Logic Error");
			// setText()
			if( hex_idx>=0 ) {
				char ch = m_retHexBa[hex_idx];
				QChar q_ch(ch);
				lb->setText( QString(q_ch) );
				--hex_idx;
			} else {
				if( s_isShowUnunsedDigit) {
					lb->setText( QString('0') );
				} else {
					lb->setText( QString("") );
				}
			}
		}
	}
}

void MainWindow::updateBinVec()
{
	int curSize = calcNeededLabeCount(E_BIN);
	//
	// Update Label
	//
	if( m_retBinBa.isEmpty() ) {
		for(int i = curSize-1; i>=0; --i)
		{
			auto label = m_BinVec.at(i);
			if( !m_is1stUpdate ) {
				label->setText("");
			}
		}
	} else {
		//
		// s_isShowUnunsedDigit
		//
		int bin_idx = m_retBinBa.size()-1;

		for(int i = curSize-1; i>=0; --i)
		{
			QLabel* lb = m_BinVec.at(i);
			Q_ASSERT_X(lb!=nullptr && lb->isVisible(), "MainWindow::updateBinVec()", "Logic Error");
			// setText()
			if( bin_idx>=0 ) {
				char ch = m_retBinBa[bin_idx];
				QChar q_ch(ch);
				lb->setText( QString(q_ch) );
				--bin_idx;
			} else {
				if( s_isShowUnunsedDigit) {
					lb->setText( QString('0') );
				} else {
					lb->setText( QString("") );
				}
			}
		}
	}
}

void MainWindow::updateDetailInfo()
{
	ui->label->setText( makeDesc() );
	// ui->label->setHtml( makeDesc() );

	QString str_min_max;
	QString strNumber_min;
	QString strNumber_max;

	// 
	if( m_signed_flag == E_UN_SIGNED::E_UNSIGNED) {
		if( m_dataType == E_CHAR) {
			unsigned char min = numeric_limits<unsigned char>::min();
			unsigned char max = numeric_limits<unsigned char>::max();
			int i_min = min;
			int i_max = max;
			strNumber_min = QString::number(i_min);
			strNumber_max = QString::number(i_max);
			str_min_max = QStringLiteral("number range : [ ") + strNumber_min  + QStringLiteral(" , ") + strNumber_max + QString(" ]");
		} else if( m_dataType == E_SHORT) {
			unsigned short min = numeric_limits<unsigned short>::min();
			unsigned short max = numeric_limits<unsigned short>::max();
			int i_min = min;
			int i_max = max;
			strNumber_min = QString::number(i_min);
			strNumber_max = QString::number(i_max);
			str_min_max = QStringLiteral("number range : [ ") +  strNumber_min  + QStringLiteral(" , ") + strNumber_max + QString(" ]");
		} else if( m_dataType == E_INT) {
			unsigned int min = numeric_limits<unsigned int>::min();
			unsigned int max = numeric_limits<unsigned int>::max();
			unsigned int i_min = min;
			unsigned int i_max = max;
			strNumber_min = QString::number(i_min);
			strNumber_max = QString::number(i_max);
			str_min_max = QStringLiteral("number range : [ ") +  strNumber_min + QStringLiteral(" , ") + strNumber_max + QString(" ]");
		} else if( m_dataType == E_LONG) {
			// (sc_LONG_SIZE>sc_INT_SIZE)
			unsigned long      min1 = numeric_limits<unsigned long>::min();
			unsigned long      max1 = numeric_limits<unsigned long>::max();
			unsigned long long min2 = numeric_limits<unsigned long long>::min();
			unsigned long long max2 = numeric_limits<unsigned long long>::max();

			if( sc_LONG_SIZE > sc_INT_SIZE ) {
				// use Type : long
				strNumber_min = QString::number(min1);
				strNumber_max = QString::number(max1);
				str_min_max = QStringLiteral("number range : [ ") + strNumber_min  + QStringLiteral(" , ") + strNumber_max + QString(" ]");
			} else {
				// use Type : long long 
				strNumber_min = QString::number(min2);
				strNumber_max = QString::number(max2);
				str_min_max = QStringLiteral("number range : [ ") +  strNumber_min + QStringLiteral(" , ") +  strNumber_max + QString(" ]");
			}
		}
	} else {
		// signed
		if( m_dataType == E_CHAR) {
			signed char min = numeric_limits<signed char>::min();
			signed char max = numeric_limits<signed char>::max();
			int i_min = min;
			int i_max = max;
			strNumber_min = QString::number(i_min);
			strNumber_max = QString::number(i_max);
			str_min_max = QStringLiteral("number range : [ ") +  strNumber_min + QStringLiteral(" , ") + strNumber_max + QString(" ]");
		} else if( m_dataType == E_SHORT) {
			signed short min = numeric_limits<signed short>::min();
			signed short max = numeric_limits<signed short>::max();
			int i_min = min;
			int i_max = max;
			strNumber_min = QString::number(i_min);
			strNumber_max = QString::number(i_max);
			str_min_max = QStringLiteral("number range : [ ") +  strNumber_min + QStringLiteral(" , ") + strNumber_max  + QString(" ]");
		} else if( m_dataType == E_INT) {
			signed int min = numeric_limits<signed int>::min();
			signed int max = numeric_limits<signed int>::max();
			signed int i_min = min;
			signed int i_max = max;
			strNumber_min = QString::number(i_min);
			strNumber_max = QString::number(i_max);
			str_min_max = QStringLiteral("number range : [ ") + strNumber_min + QStringLiteral(" , ") + strNumber_max  + QString(" ]");
		} else if( m_dataType == E_LONG) {
			signed long      min1 = numeric_limits<signed long>::min();
			signed long      max1 = numeric_limits<signed long>::max();
			signed long long min2 = numeric_limits<signed long long>::min();
			signed long long max2 = numeric_limits<signed long long>::max();
			if( sc_LONG_SIZE > sc_INT_SIZE ) {
				// use Type : long
				strNumber_min = QString::number(min1);
				strNumber_max = QString::number(max1);
				str_min_max = QStringLiteral("number range : [ ") +  strNumber_min + QStringLiteral(" , ") + strNumber_max  + QString(" ]");
			} else {
				// use Type : long long
				strNumber_min = QString::number(min2);
				strNumber_max = QString::number(max2);
				str_min_max = QStringLiteral("number range : [ ") +  strNumber_min + QStringLiteral(" , ") + strNumber_max  + QString(" ]");
			}
		}
	}

	// Set Min && Max Number
	ui->label_5->setText(str_min_max);
	ui->lineEdit_min->setText( strNumber_min );
	ui->lineEdit_max->setText( strNumber_max );

	//  Set PlaceHold Text
	if( m_dataType == E_CHAR && m_char_number != E_CHAR_NUMBER_ONLY) {
		ui->lineEdit->setPlaceholderText("Input a character,Such as \'A\'");
	} else {
		if( m_BaseMode == 2) {
			ui->lineEdit->setPlaceholderText("Input a Binary Number,Such as  101 (2)");
		} else if( m_BaseMode == 16) {
			ui->lineEdit->setPlaceholderText("Input a Hex Number,Such as 3F(16)");
		} else {
			ui->lineEdit->setPlaceholderText("Input a Decial Number(10)");
		}
	}
}



QString MainWindow::makeDesc()
{
	static const int  FACTOR_BYTE2BIT = 8;
	QString s1;
	if( m_signed_flag  == E_UN_SIGNED::E_UNSIGNED ) {
		s1 = "unsigned";
	} else {
		s1 = "signed";
	}


	QString s2;
	QString s3;
	switch(m_dataType) 
	{
	case E_DATA_TYPE::E_CHAR:
		s2 = "char";
		s3 = QString("<font color='#FF0000'><b>%1</b></font> bytes , <font color='#FF0000'><b>%2</b></font> bits").arg(sc_CHAR_SIZE).arg(sc_CHAR_SIZE * FACTOR_BYTE2BIT);
		break;
	case E_DATA_TYPE::E_SHORT:
		s2 = "short";
		s3 = QString("<font color='#FF0000'><b>%1</b></font> bytes , <font color='#FF0000'><b>%2</b></font> bits").arg(sc_SHORT_SIZE).arg(sc_SHORT_SIZE * FACTOR_BYTE2BIT);
		break;
	case E_DATA_TYPE::E_INT:
		s2 = "int";
		s3 = QString("<font color='#FF0000'><b>%1</b></font> bytes , <font color='#FF0000'><b>%2</b></font> bits").arg(sc_INT_SIZE).arg(sc_INT_SIZE * FACTOR_BYTE2BIT);
		break;
	case E_DATA_TYPE::E_LONG:
		{
			bool isLongLong = !(sc_LONG_SIZE > sc_INT_SIZE);
			size_t szOf_longOrLongLong = isLongLong ? sc_LONGLONG_SIZE : sc_LONG_SIZE;
			s2 = isLongLong ? "long long" : "long";
			s3 = QString("<font color='#FF0000'><b>%1</b></font> bytes = <font color='#FF0000'><b>%2</b></font> bits").arg(szOf_longOrLongLong).arg(szOf_longOrLongLong * FACTOR_BYTE2BIT);
		}
		break;
	default:
		break;
	}

	return s1 + " " + s2 + " -->  " + s3;
}

void MainWindow::on_pushButton_clicked()
{
	//
	// test Only
	// 
	
	// QString str("FF");
	// bool b = false;
	// int num = str.toInt(&b,16);
	// if( b) {
	//  	qDebug() << "convert OK , num = " << num;
	// } else {
	//  	qDebug() << "convert Failed , num = " << num;
	// }

	// QByteArray ary
    char buf[1] = {  0x0  };
	buf[0] = 0xFF;
	QByteArray ary(buf,1);
	QDataStream ds(&ary,QIODevice::ReadOnly);
    qint8 num = 0;
	ds >> num;
	qDebug() << "num = " << static_cast<int>(num);


	// buf[0] = 0xFF;
	// buf[1] = 0xFF;
	// int sz = sizeof(buf);
	// qDebug() << "sz = " << sz;
	// QByteArray ary = QByteArray::fromRawData(buf, sz);
	// qDebug() << "ary = " << ary;
	// for( int i = 0; i < ary.size(); ++i) {
	// 	char ch = ary.at(i);
	// 	int i_num = static_cast<int>(ch);
	// 	qDebug() << i << ". " << i_num;
	// }
	// QDataStream in(&ary,QIODevice::ReadOnly);
	// qint16 num = 0;
	// qDebug() << "sizeof(qint16) = " << sizeof(qint16);
	// qDebug() << "Before , num = " << num;
	// in >> num;
	// qDebug() << "After  , num = " << num;

	
	// qDebug() << "sizeof(long) = " << sizeof(long);
	// qDebug() << "sizeof(long long) = " << sizeof(long long);
	// qDebug() << "sizeof(qlonglong) = " << sizeof(qlonglong);
	// qDebug() << "sizeof(qulonglong) = " << sizeof(qulonglong);

	// for(int i = 0; i <=255; ++i)
	// {
	// 	char ch = static_cast<char>(i);
	// 	QChar q_ch(ch);
	// 	qDebug() << i << ". " << q_ch << " isPrint = " << q_ch.isPrint() <<  " , isPunct = " << q_ch.isPunct() << " , isSymbol = " << q_ch.isSymbol();
	// }

	// bool b = false;
	// short num = ary.toShort(&b,16);
	// if( b) {
	//  	qDebug() << "convert OK , s_num = " << num;
	// } else {
	//  	qDebug() << "convert Failed , s_num = " << num;
	// }

	// qDebug() << ary.data();

	// short s_num = -1;
	// QByteArray ary;
	// ary.setNum(s_num,16);
	// for( int i = 0; i < ary.size(); ++i)
	// {
	// 	char ch = ary[i];
	// 	qDebug() << i << ". " << ch;
	// }
	
	// QByteArray ary("7FFF");
	// bool b = false;
	// short s_num = ary.toShort(&b,16);
	// if( b) {
	//  	qDebug() << "convert OK , s_num = " << s_num;
	// } else {
	//  	qDebug() << "convert Failed , s_num = " << s_num;
	// }
	
    // QString test("-ffff");
	// bool b = false;
	// short s_num = test.toShort(&b,16);
	// if( b ) {
	// 	qDebug() << "convert OK , num = " << s_num;
	// } else {
	// 	qDebug() << "convert Failed , num = " << s_num;
	// }



	/*
	int h_visibleCount = getVisibleLableCount(&m_HexVec);
	int h_Count = m_HexVec.size();
	int h_visibleLineCount = getVisibleLableCount(&m_HexLineVec);
	int h_LineCount = m_HexLineVec.size();
	int b_visibleCount = getVisibleLableCount(&m_BinVec);
	int b_Count = m_BinVec.size();
	int b_visibleLineCount = getVisibleLableCount(&m_BinLineVec);
	int b_LineCount = m_BinLineVec.size();

	qDebug() << "==================================================";
	qDebug() << "Hex Vec's size = " << h_visibleCount << "/" << h_Count;
	qDebug() << "Hex Line Vec's size = " << h_visibleLineCount << "/" << h_LineCount;
	qDebug() << "**********";
	qDebug() << "Bin Vec's size = " << b_visibleCount << "/" << b_Count;
	qDebug() << "Bin Line Vec's size = " << b_visibleLineCount << "/" << b_LineCount;
	qDebug() << "==================================================";
	*/
}

void MainWindow::on_radioButton_clicked()
{
	// unsigned
	// qDebug() << "unsigned clicked " << endl;
	if( m_signed_flag == E_UNSIGNED) {
		return;
	}

	m_signed_flag = E_UNSIGNED;
	reArrangeUI(false);
	updateDetailInfo();
}

void MainWindow::on_radioButton_2_clicked()
{
	// signed
	// qDebug() << "signed clicked " << endl;
	if( m_signed_flag == E_SIGNED) {
		return;
	}

	m_signed_flag = E_SIGNED;
	reArrangeUI(false);
	updateDetailInfo();
}

void MainWindow::on_radioButton_3_clicked()
{
	// char
	if( m_dataType == E_CHAR ) {
		return;
	}
	m_dataType = E_CHAR;


	// By Default , Set as input char only mode
	// m_char_number = E_CHAR_ONLY;
	ui->radioButton_9->click();	// the relative set is set Decimal mode , trigger on_radioButton_9_clicked
	ui->radioButton_10->click();// set char-only , trigger on_radioButton_10_clicked
	// Disable it when input is char mode , because , char don't need   binary/hex/decial mode
	ui->groupBox_3->setEnabled(false);

	ui->groupBox_4->setVisible(m_dataType == E_CHAR);
	setDecimalLabelVisible();

	reArrangeUI(true);
	updateDetailInfo();
}

void MainWindow::on_radioButton_4_clicked()
{
	// short
	if( m_dataType == E_SHORT ) {
		return;
	}

	m_dataType = E_SHORT;
	ui->groupBox_4->setVisible(m_dataType == E_CHAR);
	ui->groupBox_3->setEnabled(true);
	setDecimalLabelVisible();

	reArrangeUI(true);
	updateDetailInfo();

}

void MainWindow::on_radioButton_5_clicked()
{
	// int
	if( m_dataType == E_INT ) {
		return;
	}

	m_dataType = E_INT;
	ui->groupBox_4->setVisible(m_dataType == E_CHAR);
	ui->groupBox_3->setEnabled(true);
	setDecimalLabelVisible();

	reArrangeUI(true);
	updateDetailInfo();

}

void MainWindow::on_radioButton_6_clicked()
{
	// long
	if( m_dataType == E_LONG ) {
		return;
	}

	m_dataType = E_LONG;
	ui->groupBox_4->setVisible(m_dataType == E_CHAR);
	ui->groupBox_3->setEnabled(true);
	setDecimalLabelVisible();

	reArrangeUI(true);
	updateDetailInfo();
}



void MainWindow::on_lineEdit_returnPressed()
{
	// qDebug() << "on_lineEdit_returnPressed";
	m_retHexBa.clear();
	m_retBinBa.clear();
	m_retDecBa.clear();

	// Calc m_retHexBa Only , then convert from Hex --> Binary
	QString content = ui->lineEdit->text();
	if( content.isEmpty() ) {
		m_retHexBa.clear();
		m_retBinBa.clear();
		m_retDecBa.clear();
	} else {
		doConvert(content);
	}

	updateHexVec();
	updateBinVec();
	updateDecimalLabel();
}


void MainWindow::reArrangeUI(bool isChanged)
{
	// m_dataType
	reArrangeHexUI(isChanged);
	reArrangeBinUI(isChanged);
}

void MainWindow::reArrangeHexUI(bool isChanged)
{
	if( isChanged ) {
		int oldSize = m_HexVec.size();
		int newSize = calcNeededLabeCount(E_HEX);
		if( oldSize < newSize) {
			// larger it
			int delta = newSize - oldSize;

			static const int WIDTH = s_WIDTH_FOR_BIN*4 + 3*s_SPACE1;
			static const int posY = 210;

			int calc_X = 0;
			for(int i = 0; i < delta; ++i) {
				QLabel* create_label = new QLabel(ui->centralWidget);
				if( i == 0) {
					QLabel* last = m_HexVec.back();
					if( last!=nullptr) {
                        calc_X = last->geometry().x() + WIDTH + s_SPACE3;
					}
				} else {
					int space = (i%2==1 ? s_SPACE2 : s_SPACE3);
					calc_X += (space + WIDTH);
				}

				if(i>0 && (i+1)%2==0 ) {
					QLabel* line_Label = new QLabel(ui->centralWidget);
					line_Label->setGeometry( QRect(calc_X+WIDTH+2,posY,5, s_HEIGHT) );
					// line_Label->setText("|");
					line_Label->setVisible(true);
					m_HexLineVec.push_back(line_Label);
				}

				// set font 
				QFont f_new( create_label->font() );
				f_new.setPointSize( f_new.pointSize() * 4);
				create_label->setFont(f_new);

				create_label->setGeometry( QRect(calc_X ,posY ,WIDTH,s_HEIGHT) );
				// create_label->setText("-");
				create_label->setVisible(true);

				m_HexVec.push_back(create_label);
			}
		} else {
			// oldSize > newSize
			for(int i = 0; i < oldSize; ++i) {
				auto label = m_HexVec.at(i);
				if( label!=nullptr) {
					if( i < newSize) {
						label->setVisible(true);
						// label->setText("-");
						if( i>0 && (i+1)%2==0 ) {
							int idx = (i+1)/2 - 1;
							if(idx>=0 && idx < m_HexLineVec.size() ) {
								auto line = m_HexLineVec.at(idx);
								if( line!=nullptr) {
									line->setVisible(true);
								}
							}
						}
					} else {
						// Hide it
						// label->setText("-");
						label->setVisible(false);

						if( i>0 && (i+1)%2==0) {
							int idx = (i+1)/2 - 1;
							if(idx>=0 && idx < m_HexLineVec.size() ) {
								auto line = m_HexLineVec.at(idx);
								if( line!=nullptr) {
									line->setVisible(false);
								}
							}
						}
					}
				}
			}
		}
	} 

	// set text as "-"
	for(auto it = m_HexVec.begin(); it!=m_HexVec.end(); ++it) {
		auto label = *it;
		if( label!=nullptr) {
			label->setText("-");
		}
	}

	// set text as "|"
	for(auto it = m_HexLineVec.begin(); it!=m_HexLineVec.end(); ++it) {
		auto label = *it;
		if( label!=nullptr) {
			label->setText("|");
		}
	}

	m_retHexBa.clear();
}

void MainWindow::reArrangeBinUI(bool isChanged)
{
	if( isChanged ) {
		int oldSize = m_BinVec.size();
		int newSize = calcNeededLabeCount(E_BIN);
		if( oldSize < newSize) {
			int delta = newSize - oldSize;
			static const int posY = 260;
			int calc_X = 0;
			for(int i = 0; i < delta; ++i) {
				QLabel* create_label = new QLabel(ui->centralWidget);
				if( i == 0) {
					auto last = m_BinVec.back();
					if( last!=nullptr) {
						calc_X = last->geometry().x() + s_WIDTH_FOR_BIN + s_SPACE3;
					}
				} else {
					int space = 0;//(i%4!=0 ? s_SPACE1 : s_SPACE);
					if( i%8 == 0 ) {
						space = s_SPACE3;
					} else {
						// i%8!=0
						if( i%4 == 0 ) {
							space = s_SPACE2;
						} else {
							space = s_SPACE1;
						}
					}
					calc_X += (space + s_WIDTH_FOR_BIN);
				}


				if(i>0 && (i+1)%8==0 ) {
					QLabel* line_Label = new QLabel(ui->centralWidget);
					line_Label->setGeometry( QRect(calc_X+ s_WIDTH_FOR_BIN +2,posY,5, s_HEIGHT) );
					// line_Label->setText("|");
					line_Label->setVisible(true);
					m_BinLineVec.push_back(line_Label);
				}

				create_label->setGeometry( QRect(calc_X ,posY ,s_WIDTH_FOR_BIN,s_HEIGHT) );
				// create_label->setText("-");
				create_label->setVisible(true);
				m_BinVec.push_back(create_label);

			}
		} else {
			// oldSize > newSize
			for(int i = 0; i < oldSize; ++i) {
				auto label = m_BinVec.at(i);
				if( label!=nullptr) {
					if( i < newSize) {
						label->setVisible(true);
						// label->setText("-");

						if( i>0 && (i+1)%8==0 ) {
							int idx = (i+1)/8 - 1;
							if( idx>=0 && idx < m_BinLineVec.size() ) {
								auto line = m_BinLineVec.at(idx);
								if( line!=nullptr) {
									line->setVisible(true);
								}
							}
						}
					} else {
						// Hide it
						// label->setText("");
						// label->setText("-");
						label->setVisible(false);

						if( i>0 && (i+1)%8==0 ) {
							int idx = (i+1)/8 - 1;
							if( idx>=0 && idx < m_BinLineVec.size() ) {
								auto line = m_BinLineVec.at(idx);
								if( line!=nullptr) {
									line->setVisible(false);
								}
							}
						}
					}
				}
			}
		}
	} 

	// set text as "-"
	for(auto it = m_BinVec.begin(); it!=m_BinVec.end(); ++it) {
		auto label = *it;
		if( label!=nullptr) {
			label->setText("-");
		}
	}

	// set text as "|"
	for(auto it = m_BinLineVec.begin(); it!=m_BinLineVec.end(); ++it) {
		auto label = *it;
		if( label!=nullptr) {
			label->setText("|");
		}
	}

	m_retBinBa.clear();
}


int MainWindow::getVisibleLableCount(QVector<QLabel*>* vec)
{
	if( vec == nullptr) {
		return 0;
	}

	int sz = 0;
	for(auto it = vec->begin(); it!=vec->end(); ++it) {
		auto label = *it;
		if( label!=nullptr && label->isVisible() ) {
			++sz;
		}
	}

	return sz;
}



void MainWindow::doConvert(const QString& content)
{
	if( m_signed_flag == E_UNSIGNED ) {
		// unsigned
		switch(m_dataType)
		{
		case E_CHAR:
			doConvertUChar(content);
			break;
		case E_SHORT:
			doConvertUShort(content);
			break;
		case E_INT:
			doConvertUInt(content);
			break;
		case E_LONG:
			doConvertULong(content);
			break;
		default:
			Q_ASSERT_X(false,"MainWindow::doConvert()","in switch-case default");
			break;
		}
	} else {
		// m_signed_flag == E_SIGNED
		switch(m_dataType)
		{
		case E_CHAR:
			doConvertSChar(content);
			break;
		case E_SHORT:
			doConvertSShort(content);
			break;
		case E_INT:
			doConvertSInt(content);
			break;
		case E_LONG:
			doConvertSLong(content);
			break;
		default:
			Q_ASSERT_X(false,"MainWindow::doConvert()","in switch-case default");
			break;
		}
	}

	
}

void MainWindow::doConvertUChar(const QString& content)
{
	if( m_char_number == E_CHAR_NUMBER_ONLY ) {
		quint8 i_num = 0;
		if( m_BaseMode == 10) {
			// Originally convert 
			// -1(10)  ==>  FFFFFFFF(16)  ==> 11111111|11111111|11111111|11111111 (2)
			// -1(16)  ==>  -1(10)		  ==> 11111111|11111111|11111111|11111111 (2)
			//----------------------------------------------------------------------------------------------------
			// -15(10) ==> FFFFFFF1(16)   ==> 11111111|11111111|11111111|11110001 (2)
			// -F(16)  ==>     -15(10)    ==> 11111111|11111111|11111111|11110001 (2)
			//----------------------------------------------------------------------------------------------------
			// 10(2)    ==> 2(10)		  ==> 00000000|00000000|00000000|00000010 (2)
			// -10(2)   ==> -2(10)		  ==> 11111111|11111111|11111111|11111110 (2)
			i_num = content.toInt(&m_convertOK, m_BaseMode);
		} else {
			// i_num = content.toInt(&m_convertOK, m_BaseMode);
			m_convertOK = checkHexBinaryValid(content);
			if( m_convertOK ) {
				// convert from string to num;
				QByteArray ary( hex_bin_convert(content) );			
				QDataStream in(&ary,QIODevice::ReadOnly);
				in >> i_num;
				// qDebug() << "Finally i_num = " << i_num;
			} else {
				ui->statusBar->showMessage("Hex Letter Must in the Range [0-9] or [A-F]", sc_StatuBarShowMsgTime);
			}
		}

		// qDebug() << "i_num = " << i_num << "\t" << " convertOK = " << m_convertOK;
		if( !m_convertOK ) {
			m_retHexBa.fill('?',2);
			m_retBinBa.fill('?',8);
			m_retDecBa.fill('?', content.size());
		} else {
			// m_retHexBa = QByteArray::number(i_num,16).toUpper();
			int uchar_min = static_cast<int>( numeric_limits<unsigned char>::min() );
			int uchar_max = static_cast<int>( numeric_limits<unsigned char>::max() );
			if( i_num>=uchar_min && i_num<=uchar_max) {
				m_retHexBa = QByteArray::number(i_num,16).toUpper();
				m_retBinBa = QByteArray::number(i_num,2);
				m_retDecBa = QByteArray::number(i_num,10);
			} else {
				qDebug() << "unsigned char : " <<  i_num << " out of range";
				m_retHexBa.fill('?',2);
				m_retBinBa.fill('?',8);
				m_retDecBa.fill('?',content.size() );
			}
		}
	} else {
		// m_char_number = E_CHAR_ONLY
		QChar ch = content[0];
		char lch = ch.toLatin1();
		// qDebug() << "lch = " << lch;
		int ascii_code = static_cast<int>(lch);
		m_retHexBa = QByteArray::number(ascii_code,16).toUpper();
		m_retBinBa = QByteArray::number(ascii_code,2);
		m_retDecBa = QByteArray::number(ascii_code,10);
	}
}

void MainWindow::doConvertSChar(const QString& content)
{
	if( m_char_number == E_CHAR_NUMBER_ONLY ) {
		qint8 i_num = 0;
		if( m_BaseMode == 10) {
			i_num = content.toInt(&m_convertOK, m_BaseMode);
		} else {
			// i_num = content.toInt(&m_convertOK, m_BaseMode);
			m_convertOK = checkHexBinaryValid(content);
			if( m_convertOK ) {
				// convert from string to num;
				QByteArray ary( hex_bin_convert(content) );			

				QDataStream in(&ary,QIODevice::ReadOnly);
				in >> i_num;
				// qDebug() << "Finally i_num = " << i_num;
			} else {
				ui->statusBar->showMessage("Hex Letter Must in the Range [0-9] or [A-F]", sc_StatuBarShowMsgTime);
			}
		}

		// qDebug() << "i_num = " << i_num << "\t" << " convertOK = " << m_convertOK;
		if( !m_convertOK ) {
			m_retHexBa.fill('?',2);
			m_retBinBa.fill('?',8);
			m_retDecBa.fill('?', content.size() );
		} else {
			// m_retHexBa = QByteArray::number(i_num,16).toUpper();
			int uchar_min = static_cast<int>( numeric_limits<signed char>::min() );
			int uchar_max = static_cast<int>( numeric_limits<signed char>::max() );
			if( i_num>=uchar_min && i_num<=uchar_max) {
				m_retHexBa = QByteArray::number(i_num,16).toUpper();
				m_retBinBa = QByteArray::number(i_num,2);
				m_retDecBa = QByteArray::number(i_num,10);
			} else {
				qDebug() << "signed char : " <<  i_num << " out of range";
				m_retHexBa.fill('?',2);
				m_retBinBa.fill('?',8);
				m_retDecBa.fill('?',content.size() );
			}
		}
	} else {
		// m_char_number = E_CHAR_ONLY
		QChar ch = content[0];
		char lch = ch.toLatin1();
		// qDebug() << "lch = " << lch;
		int ascii_code = static_cast<int>(lch);
		m_retHexBa = QByteArray::number(ascii_code,16).toUpper();
		m_retBinBa = QByteArray::number(ascii_code,2);
		m_retDecBa = QByteArray::number(ascii_code,10);
	}
}

void MainWindow::doConvertUShort(const QString& content)
{
	ushort us_num = 0;
	if( m_BaseMode == 10) {
		us_num = content.toUShort(&m_convertOK, m_BaseMode);
	} else {
		m_convertOK = checkHexBinaryValid(content);
		if( m_convertOK ) {
			// convert from string to num;
			QByteArray ary( hex_bin_convert(content) );			

			QDataStream in(&ary,QIODevice::ReadOnly);
			in >> us_num;
		} else {
			ui->statusBar->showMessage("Hex Letter Must in the Range [0-9] or [A-F]", sc_StatuBarShowMsgTime);
		}
	}

	if( !m_convertOK) {
		m_retHexBa.fill('?',4);
		m_retBinBa.fill('?',16);
		m_retDecBa.fill('?',content.size());
	} else {
		m_retHexBa = QByteArray::number((uint)us_num,16).toUpper();
		m_retBinBa = QByteArray::number((uint)us_num,2);
		m_retDecBa = QByteArray::number((uint)us_num,10);
	}
}

void MainWindow::doConvertSShort(const QString& content)
{
    short ss_num = 0;
	if( m_BaseMode == 10) {
		ss_num = content.toShort(&m_convertOK, m_BaseMode);
	} else {
		m_convertOK = checkHexBinaryValid(content);
		if( m_convertOK ) {
			// convert from string to num;
			QByteArray ary( hex_bin_convert(content) );			

			QDataStream in(&ary,QIODevice::ReadOnly);
			in >> ss_num;
		} else {
			ui->statusBar->showMessage("Hex Letter Must in the Range [0-9] or [A-F]", sc_StatuBarShowMsgTime);
		}
	}

    if( !m_convertOK) {
		m_retHexBa.fill('?',4);
		m_retBinBa.fill('?',16);
		m_retDecBa.fill('?', content.size());
	} else {
		m_retHexBa = QByteArray::number((int)ss_num,16).toUpper();
		m_retBinBa = QByteArray::number((int)ss_num,2);
		m_retDecBa = QByteArray::number((int)ss_num,10);
	}
}

void MainWindow::doConvertUInt(const QString& content)
{
	uint ui_num = 0;
	if( m_BaseMode == 10) {
		ui_num = content.toUInt(&m_convertOK, m_BaseMode);
	} else {
		m_convertOK = checkHexBinaryValid(content);
		if( m_convertOK ) {
			// convert from string to num;
			QByteArray ary( hex_bin_convert(content) );			
			QDataStream in(&ary,QIODevice::ReadOnly);
			in >> ui_num;
		} else {
			ui->statusBar->showMessage("Hex Letter Must in the Range [0-9] or [A-F]", sc_StatuBarShowMsgTime);
		}
	}

	if( !m_convertOK) {
		m_retHexBa.fill('?',8);
		m_retBinBa.fill('?',32);
		m_retDecBa.fill('?',content.size() );
	} else {
		m_retHexBa = QByteArray::number(ui_num,16).toUpper();
		m_retBinBa = QByteArray::number(ui_num,2);
		m_retDecBa = QByteArray::number(ui_num,10);
	}
}

void MainWindow::doConvertSInt(const QString& content)
{
	int i_num = 0;
	if( m_BaseMode == 10) {
		i_num = content.toInt(&m_convertOK, m_BaseMode);
	} else {
		m_convertOK = checkHexBinaryValid(content);
		if( m_convertOK ) {
			// convert from string to num;
			QByteArray ary( hex_bin_convert(content) );			

			QDataStream in(&ary,QIODevice::ReadOnly);
			in >> i_num;
		} else {
			ui->statusBar->showMessage("Hex Letter Must in the Range [0-9] or [A-F]", sc_StatuBarShowMsgTime);
		}
	}

	if( !m_convertOK ) {
		m_retHexBa.fill('?',8);
		m_retBinBa.fill('?',32);
		m_retDecBa.fill('?',content.size() );
	} else {
		m_retHexBa = QByteArray::number(i_num,16).toUpper();
		m_retBinBa = QByteArray::number(i_num,2);
		m_retDecBa = QByteArray::number(i_num,10);
	}
	
}
void MainWindow::doConvertULong(const QString& content)
{
	qulonglong ul_num = 0UL;
	if( m_BaseMode == 10) {
        ul_num = content.toULongLong(&m_convertOK, m_BaseMode);
	} else {
		m_convertOK = checkHexBinaryValid(content);
		if( m_convertOK ) {
			// convert from string to num;
			QByteArray ary( hex_bin_convert(content) );			

			QDataStream in(&ary,QIODevice::ReadOnly);
			in >> ul_num;
		} else {
			ui->statusBar->showMessage("Hex Letter Must in the Range [0-9] or [A-F]", sc_StatuBarShowMsgTime);
		}
	}

	if( !m_convertOK ) {
		m_retHexBa.fill('?',16);
		m_retBinBa.fill('?',64);
		m_retDecBa.fill('?', content.size());
	} else {
		m_retHexBa = QByteArray::number(ul_num,16).toUpper();
		m_retBinBa = QByteArray::number(ul_num,2);
		m_retDecBa = QByteArray::number(ul_num,10);
	}
}

void MainWindow::doConvertSLong(const QString& content)
{
	qlonglong sl_num = 0L;
	if( m_BaseMode == 10) {
        sl_num = content.toLongLong(&m_convertOK, m_BaseMode);
	} else {
		m_convertOK = checkHexBinaryValid(content);
		if( m_convertOK ) {
			// convert from string to num;
			QByteArray ary( hex_bin_convert(content) );			

			QDataStream in(&ary,QIODevice::ReadOnly);
			in >> sl_num;
		} else {
			ui->statusBar->showMessage("Hex Letter Must in the Range [0-9] or [A-F]", sc_StatuBarShowMsgTime);
		}
	}

	if( !m_convertOK) {
		m_retHexBa.fill('?',16);
		m_retBinBa.fill('?',64);
		m_retDecBa.fill('?',content.size() );
	} else {
		m_retHexBa = QByteArray::number(sl_num,16).toUpper();
		m_retBinBa = QByteArray::number(sl_num,2);
		m_retDecBa = QByteArray::number(sl_num,10);
	}
}


void MainWindow::on_checkBox_toggled(bool checked)
{
	s_isShowUnunsedDigit = checked;
	updateHexVec();
	updateBinVec();
}

void MainWindow::on_radioButton_7_clicked()
{
	// Binary Mode
	m_BaseMode = 2;
	setDecimalLabelVisible();
	if( m_dataType!=E_CHAR ) {
		ui->lineEdit->setPlaceholderText("Input a Binary Number,Such as  101 (2)");
	} else {
		ui->lineEdit->setPlaceholderText("Input an AscIICode,Such as 01000001-A");
	}
}

void MainWindow::on_radioButton_8_clicked()
{
	// Hex Mode
	m_BaseMode = 16;
	setDecimalLabelVisible();
	if( m_dataType!=E_CHAR ) {
		ui->lineEdit->setPlaceholderText("Input a Hex Number,Such as 3F(16)");
	} else {
		ui->lineEdit->setPlaceholderText("Input an AscII-Code,Such as 41(16)-A ");
	}
}

void MainWindow::on_radioButton_9_clicked()
{
	// Decimal Mode
	m_BaseMode = 10;
	setDecimalLabelVisible();
	if( m_dataType!=E_CHAR) {
		ui->lineEdit->setPlaceholderText("Input a Decial Number(10)");
	} else {
		ui->lineEdit->setPlaceholderText("Input an AscII-Code,Such as 65-A");
	}
}

void MainWindow::setDecimalLabelVisible()
{
	bool isShow = (m_dataType == E_CHAR || m_BaseMode!=10);
	ui->label_6->setVisible(isShow);
}

void MainWindow::updateDecimalLabel()
{
	static const QPair<char,QString> specialAry[] = {
		qMakePair('\t',QStringLiteral("\\t")),		// 0x09 <Tab>
		qMakePair('\n',QStringLiteral("\\n")),		// 0x10 <Enter> = Line Feed
		qMakePair('\r',QStringLiteral("\\r")),		// 0x0D <CR>	= Carriage Return
		qMakePair(' ',QStringLiteral("<space>"))	// 0x20 = 32 = <space>
	};

	setDecimalLabelVisible();

	QString str(" = ");
	if( m_retDecBa.isEmpty() ) {
		str = " = ??? (10)";
	} else {
		// not empty
		if( m_dataType!=E_CHAR ) {
			for(int i = 0; i < m_retDecBa.size(); ++i) {
				char ch = m_retDecBa[i];
				str += ch;
			}
		} else {
			// m_dataType == E_CHAR
			// qDebug() << "MainWindow::updateDecimalLabel() m_dataType == E_CHAR";

			int num = m_retDecBa.toInt(&m_convertOK,10);
			// qDebug() << "num = " << num;

			bool hasFind = false;
			QString paired_str;
            for(int i = 0; i < (int)(sizeof(specialAry)/sizeof(specialAry[0])); ++i) {
				char ch = specialAry[i].first;
				if( num == ch) {
					hasFind = true;
					paired_str = specialAry[i].second;
					break;
				}
			}

			str += "'";
			QChar q_ch(num);
			if( hasFind ) {
				str += paired_str;
				str += "'";
			} else if( q_ch.isPrint() ) {
				str += q_ch.toLatin1();
				str += "'";
			} else {
				str += "<un-Printable>";
				str += "'";
			}


			str += " = ";
			for(int i = 0; i < m_retDecBa.size(); ++i) {
				char ch = m_retDecBa[i];
				str += ch;
			}
		}
		str += " (10)";
	}
	ui->label_6->setText(str);
}


bool MainWindow::checkHexBinaryValid(const QString& content)
{
	bool ret = true;
	int cnt = 0;
	if( m_BaseMode == 16 ) {
		cnt = calcNeededLabeCount(E_HEX);
		if( content.size() > cnt) {
			ret = false;
		} else {
			for(int i = 0; i < content.size(); ++i) {
                QChar ch = content[0];
				bool isHexCode = (ch>=QChar('0') && ch<=QChar('9'))
					 || (ch>=QChar('A') || ch<=QChar('F'))
					 || (ch>=QChar('a') || ch<=QChar('f'));
				if(!isHexCode) {
					ret = false;
					break;
				}
			}
		}
	} else if(m_BaseMode == 2) {
		cnt = calcNeededLabeCount(E_BIN);
		if( content.size() > cnt) {
			ret = false;
		} else {
			for(int i = 0; i < content.size(); ++i) {
				QChar ch = content[0];
				bool isBinCode = ( ch==QChar('0') || ch==QChar('1') );
				if(!isBinCode) {
					ret = false;
					break;
				}
			}
		}
	} else {
		ret = true;
	}

	return ret;
}

QByteArray MainWindow::hex_bin_convert(const QString& content)
{
	QString convertStr(content);
	int cnt = calcNeededLabeCount(m_BaseMode==16 ? E_HEX : E_BIN);
	QString rightFill;
	if( convertStr.size() < cnt ) {
		rightFill = convertStr.rightJustified(cnt,'0');
	} else {
		rightFill = convertStr;
	}

	QByteArray ary;
	int groupCnt = (m_BaseMode==16 ? 2 : 8);
	int add_cnt = 0;
	for(int i = 0; i < rightFill.size()/groupCnt; ++i) {
		QString make_char = rightFill.mid(i*groupCnt,groupCnt);
		bool b = false;
		int code = make_char.toInt(&b,m_BaseMode);
		char ch = static_cast<char>(code);
		if( b) {
			// qDebug() << i << ". --> += ";
			++add_cnt;
			ary += ch;
		} 
	}

	// qDebug() << "add_cnt = " << add_cnt;
	// qDebug() << "==================================================";
	// for(int i = 0; i < ary.size(); ++i) {
	// 	char ch = ary.at(i);
	// 	int code = ch;
	// 	qDebug() << i << ". code = " << code;
	// }
	// qDebug() << "==================================================";

	return ary;
}

void MainWindow::on_radioButton_10_clicked()
{
	if( m_char_number == E_CHAR_ONLY) {
		return;
	}
	m_char_number = E_CHAR_ONLY;

    ui->lineEdit->setPlaceholderText("Input a character,Such as \'A\'");
	// Disable it when input is char mode , because , char don't need   binary/hex/decial mode
    ui->groupBox_3->setEnabled(false);
}

void MainWindow::on_radioButton_11_clicked()
{
	if( m_char_number == E_CHAR_NUMBER_ONLY ) {
		return;
	}
	m_char_number = E_CHAR_NUMBER_ONLY;
    ui->lineEdit->setPlaceholderText("Input the AscIICode,Such as 65-\'A\'");
	// Enable it
	ui->groupBox_3->setEnabled(true);
}




void MainWindow::on_btnCopyHex_clicked()
{
	auto copyBoard = qApp->clipboard();
	if( copyBoard!=nullptr ) {
		if( !m_retHexBa.isEmpty() ) {
			QChar ch0 = m_retHexBa.at(0);
            if( ch0!=QChar('?') && ch0!=QChar('-') ) {
				QString content(m_retHexBa);
				copyBoard->setText(content);
				// void QToolTip::showText(const QPoint & pos, const QString & text, QWidget * w, const QRect & rect, int msecDisplayTime)
				auto showPos = ui->btnCopyHex->mapToGlobal( QPoint(0,0) );
                showPos.ry() -= 50;
                QToolTip::showText(showPos , "Hex Code --> ClipBoard", nullptr, QRect() , sc_StatuBarShowMsgTime);
                ui->statusBar->showMessage("HexData has successfully copied to system clipBoard", sc_StatuBarShowMsgTime);
                return;
            }
		}
	}

    ui->statusBar->showMessage("Copy Hex-Data Failed", sc_StatuBarShowMsgTime);
}

void MainWindow::on_btnCopyBin_clicked()
{
	auto copyBoard = qApp->clipboard();
	if( copyBoard!=nullptr ) {
		if( !m_retBinBa.isEmpty() ) {
			QChar ch0 = m_retBinBa.at(0);
            if( ch0!=QChar('?') && ch0!=QChar('-') ) {
				QString content(m_retBinBa);
				auto showPos = ui->btnCopyBin->mapToGlobal( QPoint(0,0) );
                showPos.ry() -= 50;
				copyBoard->setText(content);
                QToolTip::showText(showPos , "Binary Code --> ClipBoard", nullptr, QRect() , sc_StatuBarShowMsgTime);
                ui->statusBar->showMessage("BinaryData has successfully copied to system clipBoard", sc_StatuBarShowMsgTime);
                return;
            }
		}
	}

    ui->statusBar->showMessage("Copy Binary-Data Failed", sc_StatuBarShowMsgTime);

}
