#ifndef _IVCAOBJECT_HH
#define _IVCAOBJECT_HH

#include "../../PlayerFactory/struct_def.h"

class IVCAObject
{
public:
	IVCAObject(){}
	virtual ~IVCAObject(){}
public:
	virtual void ProcessVCA(DISPLAYCALLBCK_INFO *pInfo) = 0;
};
#endif