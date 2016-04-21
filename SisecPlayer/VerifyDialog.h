#ifndef VerifyDialog_H
#define VerifyDialog_H

#include <QDialog>
#include <QString>
#include <QProgressDialog>
#include <QWaitCondition>
#include <QMutex>


namespace Ui {
    class VerifyDialog;
}

class VerifyDialog : public QDialog {
    Q_OBJECT
public:
    VerifyDialog(QWidget *parent = 0);
    ~VerifyDialog();

signals:

protected:
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
private slots:
    void on_pushButtonOK_clicked();

    void on_pushButtonPasswd_clicked();

private:
    Ui::VerifyDialog *ui;

};

#endif // DISKDLG_H
