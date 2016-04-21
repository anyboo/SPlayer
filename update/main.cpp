#include "cupdatedlg.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
//	argc = 2;
	//argv[1] = "http://www.sisec.com.cn/OVPlayer/update.7z";
	if (argc >= 2)
	{
		QApplication a(argc, argv);
		CUpdateDlg w;	
		w.show();
		w.downloadFile(argv[1]);
		return a.exec();
	}
	return 0;
}
