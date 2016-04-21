#include "singleapllication.h"

#include <QtNetwork/QLocalSocket>
#include <QFileInfo>
#include "cdialogmain.h"
#include <qtextcodec.h>

#include <QDomComment>

#define FILEPLAYLISTXML "playlist.xml"
#define ROOTTEXT  "playFileList"
#define NOTETEXT "fileName"

#define TIME_OUT                (500)    // 500ms
SingleApplication* SingleApplication::sInstance = nullptr;
SingleApplication::SingleApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , w(NULL)
    , _isRunning(false)
    , _localServer(NULL) {
	
	QString strPlayFile = "";
	if (argc >= 2)
	{
		char *pFile = argv[1];
		QTextCodec *textcode = QTextCodec::codecForName("GB18030");
		strPlayFile = textcode->toUnicode(pFile);
	}
/*	else
	{
		char *pFile = "D:/DownLoadVideo/59航景科技.mp4";
		QTextCodec *textcode = QTextCodec::codecForName("GB18030");
		g_strPlayFile = textcode->toUnicode(pFile);
		//g_strPlayFile = 
	}*/
	//写播放列表
	QDomDocument doc;

	QDomNode instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
	doc.appendChild(instruction);

	QDomElement root = doc.createElement("shellPlayFile");
	QDomText nodeText = doc.createTextNode("");
	root.appendChild(nodeText);
	doc.appendChild(root);

	QDomElement note = doc.createElement(NOTETEXT);
	QDomText nodeText2 = doc.createTextNode(strPlayFile);
	note.appendChild(nodeText2);
	root.appendChild(note);
	
	QString strPlayListXml = QApplication::applicationDirPath() + "/" + "shellFile.xml";
	QFile file(strPlayListXml);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
	{
		return;
	}

	QTextStream out(&file);
	out.setCodec("UTF-8");
	doc.save(out, 4, QDomNode::EncodingFromTextStream);

	file.close();


    _serverName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();

    _initLocalConnection();
    sInstance = this;
}



bool SingleApplication::isRunning() {
    return _isRunning;
}

void SingleApplication::_newLocalConnection() {
    QLocalSocket *socket = _localServer->nextPendingConnection();
    if(socket) {
        socket->waitForReadyRead(2*TIME_OUT);
        delete socket;


        _activateWindow();
    }
}


void SingleApplication::_initLocalConnection() {
    _isRunning = false;

    QLocalSocket socket;
    socket.connectToServer(_serverName);
    if(socket.waitForConnected(TIME_OUT)) {
        fprintf(stderr, "%s already running.\n",
                _serverName.toLocal8Bit().constData());
        _isRunning = true;

        return;
    }


    _newLocalServer();
}


void SingleApplication::_newLocalServer() {
    _localServer = new QLocalServer(this);
    connect(_localServer, SIGNAL(newConnection()), this, SLOT(_newLocalConnection()));
    if(!_localServer->listen(_serverName)) {
        if(_localServer->serverError() == QAbstractSocket::AddressInUseError) {
            QLocalServer::removeServer(_serverName);
            _localServer->listen(_serverName);
        }
    }
}


void SingleApplication::_activateWindow() {
    if(w) {
    //    w->showMaximized();
		
        w->raise();
        w->activateWindow();
		CDialogMain *pMainW = (CDialogMain*)w;
		pMainW->ShellExe2();
    }
}
