#ifndef CMYSLIDER_H
#define CMYSLIDER_H

#include <QSlider>
#include <QMouseEvent>

class CMySlider : public QSlider
{
	Q_OBJECT

public:
	CMySlider(QWidget *parent);
	~CMySlider();
protected:
	void mousePressEvent(QMouseEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);
	void paintEvent(QPaintEvent *ev);
private:
	
};

#endif // CMYSLIDER_H
