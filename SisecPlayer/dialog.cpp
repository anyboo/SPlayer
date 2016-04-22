#include "dialog.h"
#include "GetMac.h"
#include "GetdiskSN.h"
#include "md5.h"

#include <QMessageBox>
#include <QFile>
#include <QDataStream>
//#include "diskid.h"
#include <QCryptographicHash>
//#include "TempAuthDialog.h"


Dialog::Dialog(QWidget *parent)
	: QDialog(parent)
{

	ui.setupUi(this);
	ui.pushButtonTempAuthority->hide();
	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(ClickButn()));
    connect(ui.pushButtonTempAuthority, SIGNAL(clicked()), this, SLOT(ClickTempAuth()));
    m_strApply = getApllyCode();
    ui.lineEdit_Apply->setText(m_strApply);
}

Dialog::~Dialog()
{

}

QString Dialog::getApllyCode(){
    unsigned char address[1024];

	memset(address, 0, 1024);
#ifndef DISKSN
	if (getLocalMac(address) > 0)
#else
	if (GetHardDriveSerialNumber((char*)address,1024) > 0)
#endif
    {
        QByteArray hash_byte_array = QCryptographicHash::hash((const char*)address, QCryptographicHash::Md5);
        return hash_byte_array.toHex();
    }
    
    return "";
}

bool Dialog::hasKey()
{
	//QString strAuthorFile = QApplication::applicationDirPath() + "/" + AuthorFile;
	QFile file(AuthorFile);
    if (file.open(QIODevice::ReadOnly))
    {
        QString local;
        QDataStream in(&file);
        in >> local;
    
        file.close();
        auto md5Byte = QCryptographicHash::hash(QString("dwsfdajfaiierueiur%1adkfjaofdlasdfl").arg(getApllyCode()).toLatin1(), QCryptographicHash::Md5);
        QString md5(md5Byte.toHex());
        if (local == md5)
        {
            return true;
        }
    }

    return false;
}
void Dialog::ClickButn()
{
	QString strAuthy;
	strAuthy = ui.lineEdit_Author->text();
	
	//2af5391074f98ce09537e0c931871966 申请
	//c1c2bcb4493c40fe99ace9c72eb5123f 激活

    auto hashRe = QCryptographicHash::hash(QString("dwsfdajfaiierueiur%1adkfjaofdlasdfl").arg(m_strApply).toLatin1(), QCryptographicHash::Md5);
    QString md5 = hashRe.toHex();

    if (md5 == strAuthy)//
	{
		QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("授权成功,请重新运行程序！"), QMessageBox::Yes, QMessageBox::Yes);

		QFile file(AuthorFile);
		if (file.open(QIODevice::WriteOnly))
		{
			file.resize(0);
			QDataStream out(&file);
            out << md5;
			file.close();
		}
		else
		{
			return;
		}

		exit(0);
	}
	else
	{
		QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("授权失败"), QMessageBox::Yes, QMessageBox::Yes);
	}
	return;
}
void Dialog::ClickTempAuth()
{
//    TempAuthDialog::instance().exec();
}