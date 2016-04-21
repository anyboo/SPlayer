// PlayerAPI.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include "PlayerAPI.h"

#include "PlayerBlueSky.h"

IPlayerFactory * GetPlayerFactory()
{
	IPlayerFactory *pPlayerFactory = new CPlayerFactoryBlueSky();
	return pPlayerFactory;
}
