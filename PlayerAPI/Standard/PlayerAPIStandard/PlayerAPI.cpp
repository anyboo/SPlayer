// PlayerAPI.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "PlayerAPI.h"

#include "PlayerStandard.h"

IPlayerFactory * GetPlayerFactory()
{
	IPlayerFactory *pPlayerFacory = new CPlayerFactoryStandard();
	return pPlayerFacory;
}
