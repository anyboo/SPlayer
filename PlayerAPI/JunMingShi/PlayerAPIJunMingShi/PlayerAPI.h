#ifndef  _PLAYERAPI_DLL_H_
#define  _PLAYERFAPI_DLL_H_

#include "../../../PlayerFactory/IPlayer.h"

#ifdef __cplusplus
extern "C" {
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef PLAYERAPI_EXPORTS
#define PLAYERAPI __declspec(dllexport)
#else
#define PLAYERAPI __declspec(dllimport)
#endif


PLAYERAPI IPlayerFactory * GetPlayerFactory();


#ifdef __cplusplus
}
#endif


#endif