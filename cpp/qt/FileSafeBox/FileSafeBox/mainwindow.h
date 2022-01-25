#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


namespace Ui {
class MainWindow;
}

class MyWidget;

class QSize;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static const size_t  SC_TOTAL_CNT = 4;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual void	resizeEvent(QResizeEvent * event) Q_DECL_OVERRIDE;
	virtual void	mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
public slots:
	void onMouseEnterWidget(MyWidget* w);
	void onMouseLeaveWidget(MyWidget* w);
	void onTimeOut();

private:
    Ui::MainWindow *ui;

    MyWidget* m_lt;	// 0
    MyWidget* m_rt; // 1
    MyWidget* m_lb; // 2
    MyWidget* m_rb; // 3
	QTimer*   m_timer;


	unsigned char m_order[SC_TOTAL_CNT];
	unsigned char m_fixedTag;
	short m_current;

	void setPosAndSize(const QSize& sz);

};

#endif // MAINWINDOW_H
