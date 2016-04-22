#include "TempAuthDialog.h"
#include "ui_TempAuthDialog.h"


#include <QMessageBox>
#include <QDebug>
#include <QApplication>
#include <QDir>
#include <QTime>
#include <vector>
#include <string>
#include <memory>
#include <QByteArray>
#include <QCryptographicHash>
//#include "settings.h"
//#include "cryptopp/dll.h"
//#include "cryptopp/trap.h"
//#include "log.h"

//USING_NAMESPACE(CryptoPP)
std::shared_ptr <TempAuthDialog> TempAuthDialog::spDlg;

#define TA_LAST_FILE "tal"
#define TA_TIME_FILE "time"
const char key[] = { 0x11, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x11, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x17, 0x89, 0xab, 0xcd, 0xef };
const char iv[] = { 0x12, 0x14, 0x56, 0x78, 0x10, 0xab, 0xcd, 0xef };


TempAuthDialog::TempAuthDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TempAuthDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint|Qt::WindowSystemMenuHint| Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint);
    ui->authDateEdit->setDate(QDateTime::currentDateTime().date());
    setMouseTracking( true );


}

TempAuthDialog::~TempAuthDialog()
{

    delete ui;
}

bool TempAuthDialog::hasAuthority(){
    return getTime() > 0;
}

qlonglong TempAuthDialog::getTime(){
{
        QFile fileLast(TA_LAST_FILE);
        if (!fileLast.open(QIODevice::ReadOnly)){
    //Log::instance().AddLog(QString("%1:%2-%3 (%4)").arg(__FILE__)
    //    .arg(__LINE__).arg(__FUNCTION__).arg("file.open"));
            return 0;
        }

        QDataStream in(&fileLast);
        char* pcLast = nullptr;
        uint len = 0;
        in.readBytes(pcLast, len);
        if (pcLast == nullptr)
        {
        //    Log::instance().AddLog(QString("%1:%2-%3 (%4)").arg(__FILE__)
         //       .arg(__LINE__).arg(__FUNCTION__).arg("file.open"));
            return 0;
        }



        std::shared_ptr<char> strLast(new char[len]);
      /*  CFB_FIPS_Mode<DES_EDE3>::Decryption decryption_DES_EDE3_CFB;
        decryption_DES_EDE3_CFB.SetKeyWithIV(key, sizeof(key), iv);
        decryption_DES_EDE3_CFB.ProcessString((byte*)strLast.get(),
            (byte*)pcLast, len);
        delete pcLast;*/
        QString sLast(QString::fromLatin1(strLast.get()));

        if (sLast.toLongLong() <= 0 || QDateTime::currentDateTime().toTime_t() < sLast.toLongLong())
        {
          //  Log::instance().AddLog(QString("%1:%2-%3 (%4)").arg(__FILE__)
           //     .arg(__LINE__).arg(__FUNCTION__).arg(sLast));
            return 0;
        }
        refersh(sLast.toLongLong());
    }
    
    QFile file(TA_TIME_FILE);
    if (!file.open(QIODevice::ReadOnly)){
      //  Log::instance().AddLog(QString("%1:%2-%3 (%4)").arg(__FILE__)
       //     .arg(__LINE__).arg(__FUNCTION__).arg("file.open"));
        return 0;
    }

    QDataStream inTime(&file);
    char* pcTime = nullptr;
    uint len = 0;
    inTime.readBytes(pcTime, len);
    if (pcTime == nullptr)
    {
     //   Log::instance().AddLog(QString("%1:%2-%3 (%4)").arg(__FILE__)
       //     .arg(__LINE__).arg(__FUNCTION__).arg("pcTime == nullptr"));
        return 0;
    }

    std::shared_ptr<char> str(new char[len]);
 //   Log::instance().AddLog(QString("%1:%2-%3 (%4)").arg(__FILE__)
 //       .arg(__LINE__).arg(__FUNCTION__).arg(QString::fromLatin1(pcTime)));
 /*   CFB_FIPS_Mode<DES_EDE3>::Decryption decryption_DES_EDE3_CFB;
    decryption_DES_EDE3_CFB.SetKeyWithIV(key, sizeof(key), iv);
    decryption_DES_EDE3_CFB.ProcessString((byte*)str.get(),
        (byte*)pcTime, len);
    delete pcTime;*/
    QString sAuthoriTime=QString::fromLatin1(str.get());
   // Log::instance().AddLog(QString("%1:%2-%3 (%4)").arg(__FILE__)
    //    .arg(__LINE__).arg(__FUNCTION__).arg(sAuthoriTime));
    return sAuthoriTime.toLongLong() - QDateTime::currentDateTime().toTime_t();
}

void TempAuthDialog::refersh(qlonglong nlastTime){

    if (nlastTime > QDateTime::currentDateTime().toTime_t())
    {
        return;
    }
    
    QString sLast(QString::number(QDateTime::currentDateTime().toTime_t()));
    std::shared_ptr<char> strLast(new char[sLast.length() + 1]);
 
  /*  CFB_FIPS_Mode<DES_EDE3>::Encryption encryption_DES_EDE3_CFB;
    encryption_DES_EDE3_CFB.SetKeyWithIV(key, sizeof(key), iv);
    encryption_DES_EDE3_CFB.ProcessString((byte*)strLast.get(),
        (byte*)sLast.toStdString().data(), sLast.length() + 1);*/
//     Log::instance().AddLog(QString("%1:%2-%3 (%4)").arg(__FILE__)
//         .arg(__LINE__).arg(__FUNCTION__).arg(QString::fromLatin1(strLast.get()))    QFile fileLast(TA_LAST_FILE);
    QFile fileLast(TA_LAST_FILE);
    if (fileLast.open(QIODevice::WriteOnly))
    {
        QDataStream out(&fileLast);
        out.writeBytes(strLast.get(), sLast.length() + 1);
    }
}

void TempAuthDialog::setTime(qlonglong time){
    if (time < QDateTime::currentDateTime().toTime_t())
    {
        return;
    }
    QString sAuthoriTime(QString::number(time));
//    Log::instance().AddLog(QString("%1:%2-%3 (%4)").arg(__FILE__)
  //      .arg(__LINE__).arg(__FUNCTION__).arg(sAuthoriTime));



    std::shared_ptr<char> str(new char[sAuthoriTime.length() + 1]);
  /*  CFB_FIPS_Mode<DES_EDE3>::Encryption encryption_DES_EDE3_CFB;
    encryption_DES_EDE3_CFB.SetKeyWithIV(key, sizeof(key), iv);
    encryption_DES_EDE3_CFB.ProcessString((byte*)str.get(),
        (byte*)sAuthoriTime.toStdString().data(), sAuthoriTime.length() + 1);*/
  //  Log::instance().AddLog(QString("%1:%2-%3 (%4)").arg(__FILE__)
    //    .arg(__LINE__).arg(__FUNCTION__).arg(QString::fromLatin1(str.get())));
//    sAuthoriTime = QString::fromAscii(str.get(), sAuthoriTime.length() + 1);

    QFile file(TA_TIME_FILE);
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);
        out.writeBytes(str.get(), sAuthoriTime.length() + 1);
    }
    
    refersh(QDateTime::currentDateTime().toTime_t());
    QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("授权成功,请重新运行程序！"), QMessageBox::Yes, QMessageBox::Yes);
}

bool  TempAuthDialog::nativeEvent(const QByteArray &eventType, void *message, long *result)
{

    return QDialog::nativeEvent(eventType, message, result);
}

void TempAuthDialog::on_pushButtonOK_clicked()
{
    QString passwd = ui->lineEdit_passwords->text();
    auto md5Byte = QCryptographicHash::hash(passwd.toLatin1(), QCryptographicHash::Md5);
    QString md5(md5Byte.toHex());

//   if (!passwd.isEmpty() && md5 == Settings::getItem(KEY_PASSWORD_TA))
	if (!passwd.isEmpty()){
        QDateTime t;
        t.setDate(ui->authDateEdit->date().addDays(1));
        setTime(t.toTime_t());
        this->accept();
    }
    else{
        QMessageBox::information(nullptr, QStringLiteral("提示"), QStringLiteral("密码错误！"));
    }
}

