#include "noflamedlg.h"


NoFlameDlg::NoFlameDlg(QWidget *parent) : QDialog(parent), m_bMax(false)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Window );
//	this->setWindowFlags(Qt::CustomizeWindowHint);

}

NoFlameDlg::~NoFlameDlg()
{

}

