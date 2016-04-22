#ifndef SISECPLAYER_H
#define SISECPLAYER_H

#include <QtWidgets/QMainWindow>
#include "ui_sisecplayer.h"

class SisecPlayer : public QMainWindow
{
	Q_OBJECT

public:
	SisecPlayer(QWidget *parent = 0);
	~SisecPlayer();

private:
	Ui::SisecPlayerClass ui;
};

#endif // SISECPLAYER_H
