#pragma once
#include <QString>
class Verify
{
public:
    static const char* version();
    static QString downloadDir();
    static QString downloadDirPath();
    static QString startExe();
    static bool init();
    static void uninit();
};

