#ifndef   MY_WIDGET_H
#define   MY_WIDGET_H


#include <QWidget>

class MyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyWidget(QWidget* parent, int tag);
    virtual ~MyWidget() Q_DECL_OVERRIDE;

    int getTag();

protected:
	virtual  void  enterEvent(QEvent* event)  Q_DECL_OVERRIDE;
    virtual  void  leaveEvent(QEvent* event)  Q_DECL_OVERRIDE;
    virtual  void	paintEvent(QPaintEvent * event) Q_DECL_OVERRIDE;

signals:
    void onMouseEnter(MyWidget* self);
    void onMouseLeave(MyWidget* self);

private:
    const int m_tag;

};

#endif
