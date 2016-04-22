#include <vector>
using std::vector;
#include <Windows.h>

static bool UnicodeToMb(char *szOut, const WCHAR* wstr)
{
	int iLen = lstrlenW(wstr);
	int MBLen = WideCharToMultiByte(CP_ACP, 0, wstr, iLen, NULL, 0, NULL, NULL);
	if (MBLen <= 0)
	{
		return false;
	}

	int iRet = WideCharToMultiByte(CP_ACP, 0, wstr, iLen, szOut, MBLen, NULL, NULL);

	if (iRet != MBLen)
	{
		return false;
	}
	return true;
}

static bool MbToUnicode(WCHAR *szOut, const char *strMb)
{
	int iLen = lstrlenA(strMb);
	int iLenTmp = MultiByteToWideChar(CP_ACP, 0, strMb, iLen, NULL, 0);
	if (iLenTmp <= 0)
	{
		return false;
	}

	int iRet = MultiByteToWideChar(CP_ACP, 0, strMb, iLen, szOut, iLenTmp);

	if (iRet != iLenTmp)
	{
		return false;
	}
	return true;
}

static bool Utf8ToUnicode(WCHAR *szOut, const char* strUtf8)
{
	int iLen = lstrlenA(strUtf8);
	int iLenTmp = MultiByteToWideChar(CP_UTF8, 0, strUtf8, iLen, NULL, 0);
	if (iLenTmp <= 0)
	{
		return false;
	}

	int iRet = MultiByteToWideChar(CP_UTF8, 0, strUtf8, iLen, szOut, iLenTmp);
	if (iRet != iLenTmp)
	{
		return false;
	}
	return true;
}

static bool UnicodeToUtf8(char *szOut, const WCHAR* wstr)
{
	int iLen = lstrlenW(wstr);
	int MBLen = WideCharToMultiByte(CP_UTF8, 0, wstr, iLen, NULL, 0, NULL, NULL);
	if (MBLen <= 0)
	{
		return false;
	}

	int iRet = WideCharToMultiByte(CP_UTF8, 0, wstr, iLen, szOut, MBLen, NULL, NULL);

	if (iRet != MBLen)
	{
		return false;
	}
	return true;
}

static bool Utf8ToMb(char *szOut, const char* strUtf8)
{
	WCHAR szWchar[MAX_PATH];
	ZeroMemory(szWchar, sizeof(WCHAR)*MAX_PATH);
	bool bRet = Utf8ToUnicode(szWchar, strUtf8);
	if (!bRet)
	{
		return false;
	}

	return UnicodeToMb(szOut, szWchar);
}

static bool MbToUtf8(char *szOut, const char *strMb)
{
	WCHAR szWchar[MAX_PATH];
	ZeroMemory(szWchar, sizeof(WCHAR)*MAX_PATH);
	bool bRet = MbToUnicode(szWchar, strMb);
	if (!bRet)
	{
		return false;
	}

	return UnicodeToUtf8(szOut, szWchar);
}