// PlayerAPI.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "PlayerAPI.h"

#include "PlayerHie.h"

IPlayerFactory * GetPlayerFactory()
{
	CPlayerFactoryHie *pPlayerFacory = new CPlayerFactoryHie();
	return pPlayerFacory;
}