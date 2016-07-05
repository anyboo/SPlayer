static void (*cc_YUY2_To_RGB32)				(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);
static void (*cc_YUY2_To_RGB32_Resize_2)	(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);
static void (*cc_YUY2_To_RGB555)			(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);
static void (*cc_YUY2_To_RGB555_Resize_2)	(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);
static void (*cc_YUY2_To_RGB565)			(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);
static void (*cc_YUY2_To_RGB565_Resize_2)	(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);


void YUY2_To_RGB32				(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);
void YUY2_To_RGB32_Resize_2		(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);
void YUY2_To_RGB555				(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);
void YUY2_To_RGB555_Resize_2_2	(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);
void YUY2_To_RGB565				(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);
void YUY2_To_RGB565_Resize_2_2	(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);


void YUY2_To_RGB32_SSE			(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);
void YUY2_To_RGB32_Resize_2_SSE	(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);
void YUY2_To_RGB555_SSE			(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);
void YUY2_To_RGB555_Resize_2_SSE(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);
void YUY2_To_RGB565_SSE			(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);
void YUY2_To_RGB565_Resize_2_SSE(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);

void UYVY_To_RGB565				(BYTE* src,BYTE* dst,DWORD pitch,DWORD width,DWORD height);

void HSH_CopyYUYVImage(BYTE* pDest,BYTE* pSrc,DWORD pitch,DWORD width,DWORD height);
void HSH_CopySameFormatImage(BYTE* pDest,BYTE* pSrc,DWORD pitch,DWORD width,DWORD height,DWORD byte_pixel);
void HSH_CopyYUYVImageBy2(BYTE* pDest,BYTE* pSrc,DWORD pitch,DWORD width,DWORD height);
void HSH_CopyYUYVImageBy4(BYTE* pDest,BYTE* pSrc,DWORD pitch,DWORD width,DWORD height);

void YUY12_To_RGB565_mmx(BYTE *dst,int pitch,BYTE *y_src,BYTE *u_src,BYTE *v_src, int y_stride, int uv_stride,int width, int height);
void YUY12_To_RGB565_mmx_resize_2(BYTE *dst,int pitch,BYTE *y_src,BYTE *u_src,BYTE *v_src, int y_stride, int uv_stride,int width, int height);
void YUY12_To_RGB24_mmx(BYTE *dst,int pitch,BYTE *y_src,BYTE *u_src,BYTE *v_src, int y_stride, int uv_stride,int width, int height);
void YUY12_To_RGB24_mmx_InterleaveInvert(BYTE *dst,int pitch,BYTE *y_src,BYTE *u_src,BYTE *v_src, int y_stride, int uv_stride,int width, int height);


BOOL IsSSE2Supported();
