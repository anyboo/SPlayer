// PlayerAPI.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "PlayerAPI.h"

#include "PlayerTD.h"

IPlayerFactory * GetPlayerFactory()
{
	CPlayerFactoryTD *pPlayerFacory = new CPlayerFactoryTD();
	return pPlayerFacory;
}