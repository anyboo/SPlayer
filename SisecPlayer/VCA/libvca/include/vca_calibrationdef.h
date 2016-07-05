#ifndef _VCA_CALIBRATION_DEF_H_9869UII6235I_
#define _VCA_CALIBRATION_DEF_H_9869UII6235I_

#define MAX_NUM_CALIBBOXES 3

#define VCA_CALIB_GRID_PICK		0x01
#define VCA_CALIB_GRID_DRAG		0x02
#define VCA_CALIB_GRID_ENDDRAG	0x04
#define VCA_CALIB_GRID_NODRAW   0x08
#define VCA_CALIB_GRID_MOUSEWHEEL   0x10
#define VCA_CALIB_MOUSEOVER		0x20
#define VCA_CALIB_RULER_BEGIN	0x40
#define VCA_CALIB_RULER_END		0x80

class CALIB_MarkerBox
{
public:

	int x;
	int y;
	int height;
	float scale;
	bool bDragging;
	bool bOverheadMode;

	CALIB_MarkerBox (): 
		scale(1.0f), 
		bDragging(false),
		bOverheadMode(false)
	{

	};

	CALIB_MarkerBox (int _x, int _y, int _h, bool _bOverheadMode = false):
		bDragging(false),
		bOverheadMode(_bOverheadMode)
	{

		x = _x; 
		y = _y + _h/2;  
		height = _h; 
		scale = 1.0f; 
	
	};

	int GetLeft () 	{	return x - GetWidth()/2; };
	int GetRight () { return GetLeft() + GetWidth(); };

	int GetBottom () 
	{ 
	//	if (bOverheadMode)
	//		return y + GetHeight()/2;
	//	else
			return y; 
	};

	int GetTop () { return GetBottom() - GetHeight(); };
	int GetWidth () { 
		if (bOverheadMode)
		{
			return (int)(scale*(float)height);
		}
		else
		{
			return (int)(scale*5*(float)height/9);
		}
	};
	int GetHeight () { 	return (int)(scale*(float)height);};
	
	
	
} ;






#endif /// _VCA_CALIBRATION_DEF_H_9869UII6235I_