#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#define    WIDGETS_CNT    (4)




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
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() Q_DECL_OVERRIDE;

protected:
    virtual void	resizeEvent(QResizeEvent * event) Q_DECL_OVERRIDE;
	// virtual void	mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

    void showBDWidgets(bool show);
public slots:
	void onMouseEnterWidget(MyWidget* w);
	void onMouseLeaveWidget(MyWidget* w);
	void onTimeOut();

	void onRestoreStatuBarStyle();

private slots:
    void on_openFileButton_clicked();

    void on_encdecButton_clicked();

    // void on_saveBtn_clicked();


	void onSaveAsNormalFileAct();
	void onUpdateFileContent_WithoutChangePwdAct();
	void onUpdateFileContent_AndChangePwdAct();

private:
	void setBDWidgetPosAndSize(const QSize& sz);
    int  getWidgetTag( );

    void onBDTakeAction();

    void closeBD();
    void lauchBD();

	void showHintsMessage( const QString& msg, int tag, int msec);
	void encBufferWithPwd(const QByteArray& fileContent, const QByteArray& pwdBa, const QString& filepathToWrite = QString("") );


    void testEncDec();
    void testEncDec2();
    void getBitAry(char ch, int* bitAry8);
    void getBitAry2(char ch1, char ch2, int* bitAry16);

    QString getStrForAry8(int* ary);
    QString getStrForAry16(int* ary);


    void setFileContentLockState(bool locked, const QString& fileContent);
    void setBtnState(bool isEncryptState);

    // Test Case
    void testCaseEncAndDecFile();
private:
    Ui::MainWindow *ui;

    MyWidget* m_lt;
    MyWidget* m_rt;
    MyWidget* m_lb;
    MyWidget* m_rb;
	QTimer*   m_timer;
	QTimer*   m_timerForStatuBar;

    MyWidget* m_widgetAry[ WIDGETS_CNT];
    const QString   m_colorStyleAry[ WIDGETS_CNT ];
    QString         m_openedFileName;
    QByteArray      m_encryptedFileBuf;

	QString         m_decCodeSuccessPwd;
	QString         m_decCodeFileContent;


    const int m_hoverDuring; // in ms
    const int m_slotDoneDuring; // in ms
    const int m_passwordshowDuring; // in ms
    const int m_infoMsgShowDuring; // in ms
	const int m_statuBarRestoreOriginalStateDuring; // in ms
    const int m_attemptCntMax; // attempt the password time

    const  unsigned char m_bdOrderAry[WIDGETS_CNT];
    unsigned int m_currentTargetIdx;
    bool         m_bBDUnlocked;
    bool         m_bNextEnable;
    int          m_attemptCnt;
    int          m_btnState; // 0:Invalid ,   1: Normal File , Do Encrypt     2: Locked File , Do Decrypt ( need Password )
	


};

#endif // MAINWINDOW_H