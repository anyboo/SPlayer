#include "VerifyDialog.h"
#include "ui_VerifyDialog.h"


#include <QMessageBox>
#include <QDebug>
#include <QApplication>
#include <QDir>
#include <QTime>
#include <vector>
#include <string>
#include <memory>

#include "safedisk.h"
#include "dialogmodifypassword.h"

VerifyDialog::VerifyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VerifyDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint|Qt::WindowSystemMenuHint| Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint);

    setMouseTracking( true );
}

VerifyDialog::~VerifyDialog()
{

    delete ui;
}


bool  VerifyDialog::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    SafeDisk::instance().nativeEvent(message);

    return QDialog::nativeEvent(eventType, message, result);
}

void VerifyDialog::on_pushButtonOK_clicked()
{
    if (!SafeDisk::instance().init())
    {
        return;
    }
    if (!SafeDisk::instance().login(ui->lineEdit_passwords->text()))
    {
        return;
    }
    qDebug()<< QStringLiteral("login£¡");
    if (!SafeDisk::instance().Mount(*this))
    {
        return;
    }
    
    this->accept();
}

void VerifyDialog::on_pushButtonPasswd_clicked()
{
    DialogModifyPassword dlg(this);
    dlg.exec();
}
