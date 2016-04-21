#include "cmyslider.h"

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