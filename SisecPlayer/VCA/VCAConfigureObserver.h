#pragma once

#include "VCAConfigure.h"



class IVCAConfigureObserver
{
public:
	enum VCA_DATAMGR_EVENT
	{
		VCA_NO_CHANGE			= 0x00000000,

		// Zone Event 0x000X
		VCA_ZONE_ADD			= 0x00000001,
		VCA_ZONE_CHANGE			= 0x00000002,
		VCA_ZONE_DEL			= 0x00000004,
		VCA_ZONE_UPDATE			= 0x00000008,
		VCA_ZONE_ANY			= 0x0000000F,

		VCA_RULE_ADD			= 0x00000010,
		VCA_RULE_CHANGE			= 0x00000020,
		VCA_RULE_DEL			= 0x00000040,
		VCA_RULE_UPDATE			= 0x00000080,
		VCA_RULE_ANY			= 0x000000F0,
		
		// Counter Event 0x0X00
		VCA_COUNTER_ADD			= 0x00000100,
		VCA_COUNTER_CHANGE		= 0x00000200,
		VCA_COUNTER_DEL			= 0x00000400,
		VCA_COUNTER_UPDATE		= 0x00000800,
		VCA_COUNTER_ANY			= 0x00000F00,

		VCA_COUNTEROUT_UPDATE	= 0x00001000,

		VCA_OBJCLS_UPDATE		= 0x00010000,

		VCA_CALIB_UPDATE		= 0x00020000,

		VCA_TAMPER_UPDATE		= 0x00040000,

		VCA_ADVANCED_UPDATE		= 0x00080000,

		VCA_ENGINE_UPDATE		= 0x00100000,

		VCA_SCENECHANGE_UPDATE	= 0x00200000,

		VCA_LOAD				= 0x10000000,
		VCA_SAVE				= 0x10000000,
	};


protected:

	friend class CVCADataMgr;

	virtual void	FireOnEvent(DWORD uiEvent, DWORD uiContext) = 0;

	static BOOL IsZoneEvent(DWORD uiEvent) {
		return (uiEvent&0xf) > 0;
	}

	static BOOL IsRuleEvent(DWORD uiEvent) {
		return (uiEvent&0xf0) > 0;
	}

	static BOOL IsCounterEvent(DWORD uiEvent) {
		return (uiEvent&0xf00) > 0;
	}
	
	

};
