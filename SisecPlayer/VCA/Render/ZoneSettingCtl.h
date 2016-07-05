#pragma once

#include "VCADataMgr.h"
#include "../VCAConfigureObserver.h"


typedef enum GUI_DIRECTIONCHANGE
{
	TOP_ARROW	= 1,
	BOTTOM_ARROW,
	WHOLE_DIRECTION,
};

class CDialogVCA;
class CZoneSettingCtl : public IVCAConfigureObserver
{
public:
	CZoneSettingCtl(void);
	~CZoneSettingCtl(void);

	BOOL	Setup(CDialogVCA *pVCADlg);
	void	Reset();

	void	OnLButtonDown(UINT nFlags, POINT points);
	void	OnLButtonUp(UINT nFlags, POINT points);
	void	OnLButtonDblClk( UINT nFlags, POINT point );
	void	OnRButtonDown(UINT nFlags, POINT points);
	void	OnMouseMove(UINT nFlags, POINT points);
	void	OnMouseWheel(UINT nFlags, short zDelta, POINT points);
	void	OnChangeClientRect(RECT rect){
		m_ClientRect = rect;}
	void	OnCommand(WPARAM wParam, LPARAM lParam);

	void	FireOnEvent(DWORD uiEventType, DWORD uiContext);

	void	AddZone(BOOL bZone,bool bLineDirectA=false,bool bFullScreenColor=false);
	void	RemoveZone();
	void	AddNode();
	void	RemoveNode();
	void	AddCounter();
	void	RemoveCounter();
	void	RemoveCounters();
	void	ResetCounter();
	void	ResetCounters();
	void	RemoveZones();
	void	SetRule(VCA5_RULE_TYPE_E eRuleType);

	void	ShowMenu(POINT points, UINT uiDisplayMenuHiliteItem);
	POINT	GetCurPoint(){return m_CurPoint;}
	int      GetCurActRule(){ return m_nActiveAreaIdx; }
private:

	BOOL			m_bSetup;

	CDialogVCA		*m_pVCADialog;

	CVCADataMgr		*m_pDataMgr;
	HWND			m_hWnd;
		
	BOOL			m_bCaptured;
	POINT			m_CurCapPoint;
	POINT			m_CurPoint;

	BOOL			m_bChangingDirection;
	BOOL			m_bChangingLineCounter;

	POINT			m_cpStartDirectionPt;
	GUI_DIRECTIONCHANGE m_eDirectionStatus;

	RECT			m_ClientRect;


	VCA5_APP_AREA_T_E	m_eActiveAreaType;
	int				m_nActiveAreaIdx;
	int				m_nActiveNode;

	int				FindAreaIndex(int *piAreaIdx, VCA5_APP_AREA_T_E *pType ,POINT* point);
	int				FindNodeIndex(int ZoneIdx, POINT* point);
	BOOL			PtInZone(VCA5_APP_ZONE *pZone, POINT* point);
	
	BOOL			PtInCounter(VCA5_APP_COUNTER *pCounter, POINT* point);
	BOOL			IsOnZoneEdge(int ZoneIdx, POINT* point, int* pointIdx);

	BOOL			PtInArrow( POINT *x1, POINT	*x2, POINT cpPoint, int xdiff, int ydiff, int distdiff );
	BOOL			CheckDirectionSetting(int nZone, POINT point );
	BOOL			CheckLineCounterSetting(int nZone, POINT point);
	void			ChangeDirectionArrows( POINT point );
	void			ChangeLineCounter( int iSelectedZoneIdx, POINT point);
	BOOL			IsInsidePie( VCA5_APP_ZONE *pZone, POINT point );
	void			GetZonePie( VCA5_APP_ZONE *pZone, VCA5_POINT *pt );
	BOOL			ValidAngle( int *sangle, int *fangle );
	BOOL			ValidateZone( int ZoneIdx );
	void			GetMouseClipFromArea( HWND hWnd, unsigned int uiTotalPoints, VCA5_POINT pPoints[], POINT cpPoint, RECT *rcClient);
	void			SetAreaMouseClip( int iAreaIdx, HWND hWnd );

	void			CreateMenu();
	/*HMENU			m_hMainMenu;
	HMENU			m_hSelectedZoneMenu;
	HMENU			m_hSelectedZoneMenu_WithOutNode;
	HMENU			m_hSelectedCounterMenu;

		
	
	HMENU			m_hMenuZone;
	HMENU			m_hMenuSelectedZone;
	HMENU			m_hMenuNode;
	HMENU			m_hMenuCounter;
	HMENU			m_hMenuRule;
	HMENU			m_hMenuDisplay;*/

	void			PixelToPercent(POINT *pPoint);
	void			PercentToPixel(POINT *pPoint);


	int				m_iLineCounterSegment;
	int				m_iLineCounterLeftRight;




/*
	ToolTip			m_ttHelp;
	CPoint			m_ptHelpPnt;
	
	enum eHelpStatus
	{
		HLP_SHOW_NONE = 0,
		HLP_SHOW_CALIB,
		HLP_SHOW_ARROW

	}m_helpStatus;

	BOOL m_bShownCalibHelp, m_bShownArrowHelp;
*/
};
