/*! \file	VCATypes.h
 *  \brief	Definitions for UDP's VCAsysPC.
 *  \author	Conrad Kim
 *  \par Documentation:
 *		Conrad Kim
 *  \details	The header file is intended for use with UDP's  PC side VCA software.
 *  \ingroup	module_vcatypes
 */

#ifndef __VCA_TYPES_H__
#define __VCA_TYPES_H__

#pragma pack( push, VCATYPES_H )
#pragma pack(8)

#ifdef _WIN32
#   ifdef DLL_EXPORTS
#       define VCA_API_PREFIX  __declspec(dllexport) __stdcall
#       define VCA_API_POSTFIX
#   else
#       define VCA_API_PREFIX  __declspec(dllimport) __stdcall
#       define VCA_API_POSTFIX
#   endif // DLL_EXPORTS
#else
#   define VCA_API_PREFIX
#   define VCA_API_POSTFIX  __attribute__ (( visibility("default") ))
#endif // _WIN32

#ifndef _WIN32
#   ifndef GUID_DEFINED
#   define GUID_DEFINED
typedef struct {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} GUID;
#   endif // GUID_DEFINED
#   ifdef __cplusplus
#      define EXTERN_C    extern "C"
#   else
#      define EXTERN_C    extern
#   endif // __cplusplus
#   if !defined(INITGUID) || (defined(INITGUID) && defined(__cplusplus))
#      define GUID_EXT EXTERN_C
#   else
#      define GUID_EXT
#   endif // INITGUID
#   ifdef INITGUID
#      define DEFINE_GUID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) GUID_EXT const GUID n = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#   else
#      define DEFINE_GUID(n,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) GUID_EXT const GUID n
#   endif // INITGUID
#endif // _WIN32

#ifdef _WIN32
typedef BOOL                    VCA_BOOL;
#else
typedef int                     VCA_BOOL;
#endif
#define VCA_FALSE               0
#define VCA_TRUE                1

#ifdef _WIN32
#   ifdef  _WIN64
        typedef unsigned __int64    VCA_ULONGPTR;
#   else
        typedef _W64 unsigned int   VCA_ULONGPTR;
#   endif
#else
    typedef unsigned long           VCA_ULONGPTR;
#endif // _WIN32


#ifndef _WIN32
DEFINE_GUID( IID_IUnknown, 0x0000000, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);
struct IUnknown {
public:
    virtual long QueryInterface(const GUID& riid, void** ppvObject) = 0;
    virtual unsigned long AddRef(void) = 0;
    virtual unsigned long Release(void) = 0;
};
#endif // _WIN32

/*! \brief		Video Standards.
 *  \details	This is used to specify video standard.
 */
typedef enum {
    VCA_VF_NONE         = 0,
    VCA_VF_NTSC_M       = 1,
    VCA_VF_PAL_B        = 2,
} eVCA_VIDEO_FORMAT;

/*! \brief		VCA Color Formats.
 *  \details	This is used to specify color format of the raw image.
 */
typedef enum {
    VCA_CF_NONE     = 0x00,            //!< Color Format NONE.
    VCA_CF_RGB24    = 0x11,            //!< Composite B->G->R.
    VCA_CF_RGB16    = 0x22,            //!< Composite B->G->R, 5 bit per B & R, 6 bit per G.
    VCA_CF_RGB15    = 0x33,            //!< Composite B->G->R, 5 bit per component. **Not Supported**.
    VCA_CF_YUY2     = 0x44,            //!< Composite Y->U->Y->V (4:2:2)
    VCA_CF_UYVY     = 0x55,            //!< Composite U->Y->V->Y (4:2:2)
    VCA_CF_Y8       = 0x66,            //!< Luminance component only.
    VCA_CF_YV12     = 0xAA,            //!< Planar Y, V, U (4:2:0) (note V,U order!)
} eVCA_COLOR_FORMAT;

/*! \brief		VCA Codec Types.
 *  \details	This is used to specify type of the codec.
 */
typedef enum {
	VCA_CT_NONE     = 0,               //!< Codec Type NONE.
    VCA_CT_RAW      = 1,               //!< Not compressed raw image.
    VCA_CT_JPEG     = 2,               //!< Codec Type JPEG.
    VCA_CT_H264     = 3,               //!< Codec Type H.264.
} eVCA_CODEC_TYPE;

/*! \brief		An image information.
 *  \details	This is used to specify properties of the image.
 */
typedef struct {
	int					nWidth;        //!< Width of the image in pixels.
	int					nHeight;       //!< Height of the image in pixels.
	eVCA_COLOR_FORMAT	eColorFormat;  //!< Color format of the image.
} VCA_IMAGE_INFO;

#pragma pack( pop, VCATYPES_H  )

#endif  //__VCA_TYPES_H__
