// PlayerAPI.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "PlayerAPI.h"

#include "PlayerHB.h"

IPlayerFactory * GetPlayerFactory()
{
	CPlayerFactoryHB *pPlayerFacory = new CPlayerFactoryHB();
	return pPlayerFacory;
}
