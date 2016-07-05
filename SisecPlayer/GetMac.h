#ifndef _GET_MAC_H_
#define _GET_MAC_H_

#ifndef POINTER_64
#define	POINTER_64	__ptr64
#endif

#include <winsock.h>
#include <Iphlpapi.h>
#include <iostream>
#include <QString>
#include <QFile>
#include <QDataStream>

using namespace std;

extern QString AuthorFile;
void byte2Hex(unsigned char bData, unsigned char hex[]);
int getLocalMac(unsigned char *mac);
bool ReadAuthorizationFile(string &filedat);

#endif