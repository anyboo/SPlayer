#ifndef TempAuthDialog_H
#define TempAuthDialog_H

#include <QDialog>
#include <QString>
#include <QProgressDialog>
#include <QWaitCondition>
#include <QMutex>
#include <memory>

namespace Ui {
    class TempAuthDialog;
}

class TempAuthDialog : public QDialog {
    Q_OBJECT
private:
    static std::shared_ptr <TempAuthDialog> spDlg;
public:
    static TempAuthDialog& instance(){
        if (spDlg.get() == nullptr)
        {
            spDlg = std::make_shared<TempAuthDialog>();
        }
        
        return *spDlg;
    }

    TempAuthDialog(QWidget *parent = 0);
    ~TempAuthDialog();
    bool hasAuthority();
signals:

protected:
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    qlonglong getTime();
    void refersh(qlonglong nlastTime);
    void setTime(qlonglong time);
private slots:
    void on_pushButtonOK_clicked();

private:

    Ui::TempAuthDialog *ui;
    
};

#endif // DISKDLG_H
