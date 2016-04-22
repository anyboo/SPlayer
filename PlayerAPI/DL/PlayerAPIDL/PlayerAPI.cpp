// PlayerAPI.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "PlayerAPI.h"

#include "PlayerDL.h"

IPlayerFactory * GetPlayerFactory()
{
	CPlayerFactoryDL *pPlayerFacory = new CPlayerFactoryDL();
	return pPlayerFacory;
}