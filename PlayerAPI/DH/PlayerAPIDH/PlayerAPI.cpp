// PlayerAPI.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "PlayerAPI.h"

#include "PlayerDH.h"

IPlayerFactory * GetPlayerFactory()
{
	IPlayerFactory *pPlayerFactory = new CPlayerFactoryDH();
	return pPlayerFactory;
}
