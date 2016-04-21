#ifndef NOFLAMEDLG_H
#define NOFLAMEDLG_H
#include <QDialog>
#include <qevent.h>

class NoFlameDlg : public QDialog
{
    Q_OBJECT
public:
    NoFlameDlg(QWidget *parent = 0);
    ~NoFlameDlg();
	bool m_bMax;
protected:
    bool mDrag;
    QPoint mDragPosition;
	
    void mousePressEvent(QMouseEvent *event)
    {
		if (m_bMax)
		{
			QDialog::mousePressEvent(event);
			return;
		}
		if (event->button() == Qt::LeftButton) {
            mDrag = true;
            mDragPosition = event->globalPos() - this->pos();
            event->accept();
        }
		else
		{
			QDialog::mousePressEvent(event);
		}
    }

    void mouseMoveEvent(QMouseEvent *event)
    {
		if (m_bMax)
		{
			QDialog::mouseMoveEvent(event);
			return;
		}
		if (mDrag && (event->buttons() && Qt::LeftButton)) {
            move(event->globalPos() - mDragPosition);
            event->accept();
        }
		else
		{
			QDialog::mouseMoveEvent(event);
		}
    }

	void mouseReleaseEvent(QMouseEvent *e)
	{
		if (m_bMax)
		{
			QDialog::mouseReleaseEvent(e);
			return;
		}

		mDrag = false;
		
    }
};

#endif // NOFLAMEDLG_H
