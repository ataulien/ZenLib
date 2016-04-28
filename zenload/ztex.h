/*++

Module Name:

    ztex.h

Abstract:

    Definitions for compressed ZenGin Textures (.tex)

Author:

    Nico Bendlin <nicode@gmx.net>

Revision History:

    2005-04-25  Final/reviewed release (1.0)
    2005-04-04  First public release (draft)

--*/

#ifndef ZTEX_H_INCLUDE_GUARD
#define ZTEX_H_INCLUDE_GUARD

#include <inttypes.h>

namespace ZenLoad
{

	/* ZenGin Texture - Render Formats */
	typedef enum _ZTEX_FORMAT {
		ZTEXFMT_B8G8R8A8,  /* 0, 32-bit ARGB pixel format with alpha, using 8 bits per channel */
		ZTEXFMT_R8G8B8A8,  /* 1, 32-bit ARGB pixel format with alpha, using 8 bits per channel */
		ZTEXFMT_A8B8G8R8,  /* 2, 32-bit ARGB pixel format with alpha, using 8 bits per channel */
		ZTEXFMT_A8R8G8B8,  /* 3, 32-bit ARGB pixel format with alpha, using 8 bits per channel */
		ZTEXFMT_B8G8R8,    /* 4, 24-bit RGB pixel format with 8 bits per channel */
		ZTEXFMT_R8G8B8,    /* 5, 24-bit RGB pixel format with 8 bits per channel */
		ZTEXFMT_A4R4G4B4,  /* 6, 16-bit ARGB pixel format with 4 bits for each channel */
		ZTEXFMT_A1R5G5B5,  /* 7, 16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha */
		ZTEXFMT_R5G6B5,    /* 8, 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue */
		ZTEXFMT_P8,        /* 9, 8-bit color indexed */
		ZTEXFMT_DXT1,      /* A, DXT1 compression texture format */
		ZTEXFMT_DXT2,      /* B, DXT2 compression texture format */
		ZTEXFMT_DXT3,      /* C, DXT3 compression texture format */
		ZTEXFMT_DXT4,      /* D, DXT4 compression texture format */
		ZTEXFMT_DXT5,      /* E, DXT5 compression texture format */
		ZTEXFMT_COUNT
	} ZTEX_FORMAT;

	/* ZenGin Texture - Info Block */
	typedef struct _ZTEX_INFO {
		uint32_t   Format;     /* ZTEXFMT_ */
		uint32_t   Width;      /* mipmap 0 */
		uint32_t   Height;     /* mipmap 0 */
		uint32_t   MipMaps;    /* 1 = none */
		uint32_t   RefWidth;   /* ingame x */
		uint32_t   RefHeight;  /* ingame y */
		uint32_t   AvgColor;   /* A8R8G8B8 */
	} ZTEX_INFO;

	/* ZenGin Texture - File Signature */
#if !defined(__BIG_ENDIAN__) && !defined(_MAC)
#define ZTEX_FILE_SIGNATURE 0x5845545A  /* 'XETZ' (little-endian) */
#else
#define ZTEX_FILE_SIGNATURE 0x5A544558  /* 'ZTEX' (big-endian) */
#endif

/* ZenGin Texture - File Version */
#define ZTEX_FILE_VERSION_0 0x00000000

/* ZenGin Texture - File Header */
	typedef struct _ZTEX_FILE_HEADER {
		uint32_t   Signature;  /* ZTEX_FILE_SIGNATURE */
		uint32_t   Version;    /* ZTEX_FILE_VERSION_0 */
		ZTEX_INFO       TexInfo;
	} ZTEX_FILE_HEADER;

	/* ZenGin Texture - Palette Entry */
	typedef struct _ZTEX_PAL_ENTRY {
		uint8_t   r;
		uint8_t   g;
		uint8_t   b;
	} ZTEX_PAL_ENTRY;

	/* ZenGin Texture - Number of Palette Entries */
#define ZTEX_PAL_ENTRIES 0x0100

/* ZenGin Texture - Stored Palette */
	typedef struct _ZTEX_PALETTE {
		ZTEX_PAL_ENTRY  Entries[ZTEX_PAL_ENTRIES];
	} ZTEX_PALETTE;


	/* OpenZE additions */
#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) |   \
                ((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))
#endif

	enum {
		DDSD_CAPS = 0x00000001l,
		DDSD_HEIGHT = 0x00000002l,
		DDSD_WIDTH = 0x00000004l,
		DDSD_PITCH = 0x00000008l,
		DDSD_ALPHABITDEPTH = 0x00000080l,
		DDSD_PIXELFORMAT = 0x00001000l,
		DDSD_MIPMAPCOUNT = 0x00020000l,
		DDSD_LINEARSIZE = 0x00080000l,
		DDSD_DEPTH = 0x00800000l
	};

	enum {
		DDSCAPS_ALPHA = 0x00000002l, // alpha only surface
		DDSCAPS_COMPLEX = 0x00000008l, // complex surface structure
		DDSCAPS_TEXTURE = 0x00001000l, // used as texture (should always be set)
		DDSCAPS_MIPMAP = 0x00400000l  // Mipmap present
	};

	typedef struct tagDDPIXELFORMAT {
		uint32_t dwSize;	// size of this structure (must be 32)
		uint32_t dwFlags;	// see DDPF_*
		uint32_t dwFourCC;
		uint32_t dwRGBBitCount;	// Total number of bits for RGB formats
		uint32_t dwRBitMask;
		uint32_t dwGBitMask;
		uint32_t dwBBitMask;
		uint32_t dwRGBAlphaBitMask;
	} DDPIXELFORMAT;

	typedef struct tagDDCAPS2 {
		uint32_t dwCaps1;	// Zero or more of the DDSCAPS_* members
		uint32_t dwCaps2;	// Zero or more of the DDSCAPS2_* members
		uint32_t dwReserved[2];
	} DDCAPS2;

	enum {
		DDPF_ALPHAPIXELS = 0x00000001l,	// surface has alpha channel
		DDPF_ALPHA = 0x00000002l,	// alpha only
		DDPF_FOURCC = 0x00000004l,	// FOURCC available
		DDPF_RGB = 0x00000040l,	// RGB(A) bitmap
		DDPF_PALETTEINDEXED8 = 0x00000020l
	};

	typedef struct tagDDSURFACEDESC2 {
		uint32_t dwSize;	// size of this structure (must be 124)
		uint32_t dwFlags;	// combination of the DDSS_* flags
		uint32_t dwHeight;
		uint32_t dwWidth;
		uint32_t dwPitchOrLinearSize;
		uint32_t dwDepth;	// Depth of a volume texture
		uint32_t dwMipMapCount;
		uint32_t dwReserved1[11];
		DDPIXELFORMAT ddpfPixelFormat;
		DDCAPS2 ddsCaps;
		uint32_t dwReserved2;
	} DDSURFACEDESC2;

	struct ozRGBQUAD {
		uint8_t    rgbBlue;
		uint8_t    rgbGreen;
		uint8_t    rgbRed;
		uint8_t    rgbReserved;
	};

	struct ozRGBTRIPLE {
		uint8_t    rgbtBlue;
		uint8_t    rgbtGreen;
		uint8_t    rgbtRed;
	};
}
#endif /* ZTEX_H_INCLUDE_GUARD */
