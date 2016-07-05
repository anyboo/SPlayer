#include "cmyslider.h"
#include <QPainter>
CMySlider::CMySlider(QWidget *parent)
	: QSlider(parent)
{

}

CMySlider::~CMySlider()
{

}

void CMySlider::mousePressEvent(QMouseEvent *ev) 
{
	QSlider::mousePressEvent(ev);
	
	if ((ev->button() & Qt::LeftButton) && this->isEnabled()) 
	{
		int dur = this->maximum() - this->minimum();
		int pos = this->minimum() + dur * ((double)ev->x() / (double)this->width());
		if (pos != this->sliderPosition()) {
			this->setValue(pos);
			emit sliderPressed();
		}
	}
	
}

void CMySlider::mouseMoveEvent(QMouseEvent *ev) 
{
	QSlider::mouseMoveEvent(ev);
	/*int buftton = ev->button();
	if ((ev->button() & Qt::LeftButton) && this->isEnabled()) 
	{
		int dur = this->maximum() - this->minimum();
		int pos = this->minimum() + dur * ((double)ev->x() / this->width());
		if (pos != this->sliderPosition()) {
			this->setValue(pos);
		}
	}*/
}

void CMySlider::paintEvent(QPaintEvent *ev)
{	
	QSlider::paintEvent(ev);
/*	QPainter painter(this);
	QRect rcSlider = this->geometry();
	QRect rc = rcSlider;
	rc.setTop(0);
	rc.setBottom(rc.bottom()-2);
	rc.setLeft(100);
	rc.setRight(200);
	painter.fillRect(rc,QColor(255,0,0));*/
}