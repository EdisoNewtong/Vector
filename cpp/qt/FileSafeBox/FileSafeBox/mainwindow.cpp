#include <QDebug>
#include <QSize>
#include <QTimer>
#include <QTime>
#include <QResizeEvent>


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MyWidget.h"
#include "my_encrypt.h"
#include "my_decrypt.h"

// static 
// const int  QMainWindow::SC_TOTAL_CNT = 4;

MainWindow::MainWindow(QWidget *parent) :
      QMainWindow(parent)
    , ui(new Ui::MainWindow)
	, m_lt(nullptr)
	, m_rt(nullptr)
	, m_lb(nullptr)
	, m_rb(nullptr)
	, m_timer(nullptr)
{
    ui->setupUi(this);

	m_current = -1;
	m_order[0] = 0;
	m_order[1] = 3;
	m_order[2] = 2;
	m_order[3] = 1;

	m_fixedTag = 0x0;

	m_lt = new MyWidget(this);
	m_rt = new MyWidget(this);
	m_lb = new MyWidget(this);
	m_rb = new MyWidget(this);

    // setFixedSize(800,600);

    m_lt->setStyleSheet("QWidget { background : red; }");
    // m_lt->setStyleSheet("QWidget { background : rga(240,240,240); }");

	// m_lt->show();
    // m_lt->hide();
    connect(m_lt, &MyWidget::onMouseEnter, this, &MainWindow::onMouseEnterWidget);
	connect(m_lt, &MyWidget::onMouseLeave, this, &MainWindow::onMouseLeaveWidget);

    m_rt->setStyleSheet("QWidget { background : green; }");
	// m_rt->show();
	// m_rt->hide();
	connect(m_rt, &MyWidget::onMouseEnter, this, &MainWindow::onMouseEnterWidget);
	connect(m_rt, &MyWidget::onMouseLeave, this, &MainWindow::onMouseLeaveWidget);

    m_lb->setStyleSheet("QWidget { background : blue; }");
	// m_lb->show();
	// m_lb->hide();
	connect(m_lb, &MyWidget::onMouseEnter, this, &MainWindow::onMouseEnterWidget);
	connect(m_lb, &MyWidget::onMouseLeave, this, &MainWindow::onMouseLeaveWidget);

    m_rb->setStyleSheet("QWidget { background : yellow; }");
	// m_rb->show();
	// m_rb->hide();
	connect(m_rb, &MyWidget::onMouseEnter, this, &MainWindow::onMouseEnterWidget);
	connect(m_rb, &MyWidget::onMouseLeave, this, &MainWindow::onMouseLeaveWidget);

	setPosAndSize( this->size() );

	m_timer = new QTimer(this);
	m_timer->setSingleShot(true);
    m_timer->setInterval(2500);
	connect(m_timer, &QTimer::timeout,  this, &MainWindow::onTimeOut );
}

MainWindow::~MainWindow()
{
    delete ui;
}


// virtual
void	MainWindow::resizeEvent(QResizeEvent * event) // Q_DECL_OVERRIDE;
{
    QMainWindow::resizeEvent(event);

	setPosAndSize(event->size() );
}


// virtual 
void	MainWindow::mouseMoveEvent(QMouseEvent * event) // Q_DECL_OVERRIDE;
{
    QMainWindow::mouseMoveEvent(event);
	m_timer->stop();
}


void MainWindow::setPosAndSize(const QSize& sz)
{
    static const QSize fixedsz(15,15);

    m_lt->setGeometry(0,0, fixedsz.width(),fixedsz.height() );
    m_rt->setGeometry( sz.width() - fixedsz.width(),0, fixedsz.width(), fixedsz.height() );
    m_lb->setGeometry( 0,sz.height() - fixedsz.width(), fixedsz.width(),fixedsz.height() );
    m_rb->setGeometry( sz.width() - fixedsz.width(),sz.height() - fixedsz.width(), fixedsz.width(),fixedsz.width());
}


void MainWindow::onMouseEnterWidget(MyWidget* w)
{
    char ch1 = 'a';
    char newChar = encryptUtil::enc1(ch1);
    char backtoOriginChar = decryptUtil::dec1(newChar);
    Q_UNUSED(backtoOriginChar)


	if( w!=nullptr) {
		int idx = -1;
		if( w == m_lt ) {
			idx = 0;
			// qDebug() << "Enter Left-Top";
		} else if( w == m_rt ) {
			idx = 1;
			// qDebug() << "Enter Right-Top";
		} else if( w == m_lb ) {
			idx = 2;
			// qDebug() << "Enter Left-Bottom";
		} else if( w == m_rb ) {
			idx = 3;
			// qDebug() << "Enter Right-Bottom";
		}

		if( idx>=0 && idx<SC_TOTAL_CNT ) {
			if( m_current == -1) {
				// 1st. match condition
				if( idx == m_order[0] ) {
				   // m_current = 0;
                   qDebug() << "Started";
				   m_timer->start();
				} else {
				   m_timer->stop();
				}
			} else {
				if( m_current < SC_TOTAL_CNT ) {
					// m_current !=-1 
				}
				// ++m_current;
			}
		}
	}
}

void MainWindow::onMouseLeaveWidget(MyWidget* w)
{
	if( w!=nullptr) {
		int idx = -1;
		if( w == m_lt ) {
			idx = 0;
			// qDebug() << "Leave Left-Top";
		} else if( w == m_rt ) {
			idx = 1;
			// qDebug() << "Leave Right-Top";
		} else if( w == m_lb ) {
			idx = 2;
			// qDebug() << "Leave Left-Bottom";
		} else if( w == m_rb ) {
			idx = 3;
			// qDebug() << "Leave Right-Bottom";
		}

		if( idx>=0 && idx<SC_TOTAL_CNT ) {
			m_timer->stop();

			if( m_current == -1) {
				// 1st. match condition
                //m_current = 0;
			} else {
                //++m_current;
			}
		}

	}
}


void MainWindow::onTimeOut()
{
	auto cTime = QTime::currentTime();
    qDebug() << "Time out , Now is : " << cTime;
	// if( m_current == 0 ) {
	// 	m_fixedTag |= 0x1;
	// } else if( m_current == 1) {
	// 	m_fixedTag |= 0x2;
	// } else if( m_current == 2) {
	// 	m_fixedTag |= 0x4;
	// } else if( m_current == 3) {
	// 	m_fixedTag |= 0x8;
	// 	m_current = 4;
	// }
}
