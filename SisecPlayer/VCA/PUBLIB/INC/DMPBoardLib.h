#ifndef _DMPBOARDLIB_H_
#define _DMPBOARDLIB_H_

#include "Cmn5BoardLibEx.h"
#include "Cod5BoardLibEx.h"

#pragma pack( push, _DMPBOARDLIB_H_ )
#pragma pack(8)


#define DMP_SCALE_OPT_1_1	((0<<4)|0)
#define DMP_SCALE_OPT_1_2	((0<<4)|1)
#define DMP_SCALE_OPT_1_3	((0<<4)|2)
#define DMP_SCALE_OPT_1_4	((0<<4)|3)

#define DMP_SCALE_OPT_2_3	((1<<4)|2)
#define DMP_SCALE_OPT_3_4	((2<<4)|3)

#define DMP_SCALE_OPT_ARBITRARY	((15<<4)|15)

#define DMP_DEC_OPT_NOCAP			(1<<8)
#define DMP_DEC_OPT_EXTOUT_REVFIELD	((1<<9)|(1<<10))

#endif