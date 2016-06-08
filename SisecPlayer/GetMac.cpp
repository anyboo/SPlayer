#include "GetMac.h"

#pragma comment(lib,"Iphlpapi.lib")
#include <QNetworkInterface>

void byte2Hex(unsigned char bData, unsigned char hex[])
{
	int high = bData / 16, low = bData % 16;
	hex[0] = (high < 10) ? ('0' + high) : ('A' + high - 10);
	hex[1] = (low < 10) ? ('0' + low) : ('A' + low - 10);
}

int getLocalMac(unsigned char *mac) //获取本机MAC址 
{
#ifdef LocalMac
	QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
	QList<QNetworkInterface>::iterator it = list.begin();
	for (it; it != list.end();it++)
	{	
		QString str = it->humanReadableName();
		if (QStringLiteral("本地连接") == it->humanReadableName()){
			QString addr = it->hardwareAddress();
			addr.replace(":","");
			strcpy((char*)mac, addr.toLatin1().data());
			int len = addr.length();
			return len;
		
		}
	}
	return -1;
#else
	ULONG ulSize = 0;
	PIP_ADAPTER_INFO pInfo = NULL;
	int temp = 0;
	temp = GetAdaptersInfo(pInfo, &ulSize);//第一处调用，获取缓冲区大小
	pInfo = (PIP_ADAPTER_INFO)malloc(ulSize);
	temp = GetAdaptersInfo(pInfo, &ulSize);

	int iCount = 0;
	while (pInfo)//遍历每一张网卡
	{
		//  pInfo->Address MAC址
		for (int i = 0; i < (int)pInfo->AddressLength; i++)
		{
			byte2Hex(pInfo->Address[i], &mac[iCount]);
			iCount += 2;
			if (i < (int)pInfo->AddressLength - 1)
			{
				//mac[iCount++] = '-';
			}
			else
			{
				mac[iCount++] = '#';
			}
		}
		pInfo = pInfo->Next;
	}

	if (iCount > 0)
	{
		mac[--iCount] = '\0';
		return iCount;
	}
	else return -1;
	
#endif
}

bool ReadAuthorizationFile(string &filedat)
{
	QFile file(AuthorFile);
	QString str;
	if (file.open(QIODevice::ReadOnly))//QIODevice ReadOnly
	{
		QDataStream stream(&file);
		stream >> str;
		filedat = str.toStdString();
		file.close();
	}
	else
	{
		return false;
	}

	return true;
}