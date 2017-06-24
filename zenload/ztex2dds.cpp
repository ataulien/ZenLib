/**
 * File modified for openZE. Based on NicoDEs ztextools.
 */

#include "ztex.h"
#include <string>
#include <inttypes.h>
#include <algorithm>
#include "utils/logger.h"
#include <string.h>
#include "ztex2dds.h"
#include <squish.h>
#include <math.h>

#define ZTEX2DDS_ERROR_NONE   0 /* No Error                  */
#define ZTEX2DDS_ERROR_ARGS   1 /* Invalid Params / Syntax   */
#define ZTEX2DDS_ERROR_OPEN   2 /* Failed to Open Input      */
#define ZTEX2DDS_ERROR_READ   4 /* Failed to Read Input      */
#define ZTEX2DDS_ERROR_CREATE 5 /* Failed to Create Output   */
#define ZTEX2DDS_ERROR_WRITE  6 /* Failed to Write Output    */
#define ZTEX2DDS_ERROR_FORMAT 7 /* Invalid Input File Format */

namespace ZenLoad
{

	/**
	 * @brief Returns whether x is a power of two
	 */
	bool IsPowerOfTwo(unsigned long x)
	{
		if(x == 0)
			return false;
		while((x & 1) == 0)
			x >>= 1;
		return (1 == x);
	}

	/*FIXME: Scanline Alignment */
	uint32_t GetMipmapSize(unsigned long format, unsigned long width, unsigned long height, int level)
	{
		unsigned long x;
		unsigned long y;
		int i;

		x = std::max(1ul, width);
		y = std::max(1ul, height);
		for(i = 0; i < level; i++)
		{
			if(x > 1)
				x >>= 1;
			if(y > 1)
				y >>= 1;
		}

		switch(format)
		{
		case ZTEXFMT_B8G8R8A8:
		case ZTEXFMT_R8G8B8A8:
		case ZTEXFMT_A8B8G8R8:
		case ZTEXFMT_A8R8G8B8:
			return x * y * 4;
		case ZTEXFMT_B8G8R8:
		case ZTEXFMT_R8G8B8:
			return x * y * 3;
		case ZTEXFMT_A4R4G4B4:
		case ZTEXFMT_A1R5G5B5:
		case ZTEXFMT_R5G6B5:
			return x * y * 2;
		case ZTEXFMT_P8:
			return x * y;
		case ZTEXFMT_DXT1:
			return std::max(1ul, x / 4) * std::max(1ul, y / 4) * 8;
		case ZTEXFMT_DXT2:
		case ZTEXFMT_DXT3:
		case ZTEXFMT_DXT4:
		case ZTEXFMT_DXT5:
			return std::max(1ul, x / 4) * std::max(1ul, y / 4) * 16;
		default:
			return 0;
		}
	}

	/**
	 * @brief Utility function to read data from a vector safely
	 */
	bool readVectorData(void* target, size_t start, size_t size, const std::vector<uint8_t>& data)
	{
		if(start + size > data.size())
			return false;

		memcpy(target, &data[start], size);

		return true;
	}

	/**
	 * @brief Utility function to write to the end of a vector
	 */
	bool writeVectorData(void* data, size_t size, std::vector<uint8_t>& target)
	{
		uint8_t* bytes = reinterpret_cast<uint8_t*>(data);
		for(size_t i = 0; i < size; i++)
			target.emplace_back(bytes[i]);

		return true;
	}

	/**
	 * @brief Modified ZTEX to DDS conversion
	 */
	int convertZTEX2DDS(const std::vector<uint8_t>& ztexData, std::vector<uint8_t>& ddsData, bool optionForceARGB, int* pOutWidth, int* pOutHeight)
	{
		ZTEX_FILE_HEADER ZTexHeader;
		uint32_t BytesRead = 0;
		uint32_t DdsMagic;
		uint32_t BytesWritten;
		DDSURFACEDESC2 DdsHeader;
		int MipmapCount;
		int MipmapLevel;
		int i;
		ozRGBQUAD palentry;
		uint32_t BufferSize;
		uint8_t* Buffer;
		uint32_t MipmapSize;
		uint8_t* Mipmap;
		ozRGBQUAD * Pixel32;
		ozRGBTRIPLE * Pixel24;
		uint8_t ColorTemp;

		// Read header
		if(!readVectorData(&ZTexHeader, 0, sizeof(ZTexHeader), ztexData))
			return ZTEX2DDS_ERROR_READ;

		if(pOutWidth)
			*pOutWidth = ZTexHeader.TexInfo.Width;

		if(pOutHeight)
			*pOutHeight = ZTexHeader.TexInfo.Height;

		BytesRead += sizeof(ZTexHeader);

		/* 'ZTEX' */
		if(ZTexHeader.Signature != ZTEX_FILE_SIGNATURE ||
			ZTexHeader.Version != ZTEX_FILE_VERSION_0 ||
			ZTexHeader.TexInfo.Format > ZTEXFMT_DXT5)
		{
			return ZTEX2DDS_ERROR_FORMAT;
		}

		/* 'DDS ' */
		DdsMagic = MAKEFOURCC('D', 'D', 'S', ' ');
		if(!writeVectorData(&DdsMagic, sizeof(DdsMagic), ddsData))
		{
			return ZTEX2DDS_ERROR_WRITE;
		}

		/* DDSURFACEDESC2 */
		memset(&DdsHeader, 0, sizeof(DdsHeader));
		DdsHeader.dwSize = sizeof(DDSURFACEDESC2);
		DdsHeader.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_CAPS;
		DdsHeader.ddsCaps.dwCaps1 = DDSCAPS_TEXTURE;
		DdsHeader.dwHeight = ZTexHeader.TexInfo.Height;
		DdsHeader.dwWidth = ZTexHeader.TexInfo.Width;
		if(ZTexHeader.TexInfo.Format <= ZTEXFMT_P8)
		{
			DdsHeader.dwFlags |= DDSD_PITCH;
			DdsHeader.dwPitchOrLinearSize =
				GetMipmapSize(ZTexHeader.TexInfo.Format, DdsHeader.dwWidth, 1, 0);
		}
		else
		{
			DdsHeader.dwFlags |= DDSD_LINEARSIZE;
			DdsHeader.dwPitchOrLinearSize =
				GetMipmapSize(ZTexHeader.TexInfo.Format, DdsHeader.dwWidth, DdsHeader.dwHeight, 0);
		}
		if(ZTexHeader.TexInfo.MipMaps > 1)
		{
			DdsHeader.dwFlags |= DDSD_MIPMAPCOUNT;
			DdsHeader.ddsCaps.dwCaps1 |= DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
			DdsHeader.dwMipMapCount = ZTexHeader.TexInfo.MipMaps;
		}
		DdsHeader.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		switch(ZTexHeader.TexInfo.Format)
		{
		case ZTEXFMT_B8G8R8A8:
			DdsHeader.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			DdsHeader.ddpfPixelFormat.dwRGBBitCount = 32;
			DdsHeader.ddpfPixelFormat.dwRBitMask = 0x0000FF00;
			DdsHeader.ddpfPixelFormat.dwGBitMask = 0x00FF0000;
			DdsHeader.ddpfPixelFormat.dwBBitMask = 0xFF000000;
			DdsHeader.ddpfPixelFormat.dwRGBAlphaBitMask = 0x000000FF;
			break;
		case ZTEXFMT_R8G8B8A8:
			DdsHeader.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			DdsHeader.ddpfPixelFormat.dwRGBBitCount = 32;
			DdsHeader.ddpfPixelFormat.dwRBitMask = 0xFF000000;
			DdsHeader.ddpfPixelFormat.dwGBitMask = 0x00FF0000;
			DdsHeader.ddpfPixelFormat.dwBBitMask = 0x0000FF00;
			DdsHeader.ddpfPixelFormat.dwRGBAlphaBitMask = 0x000000FF;
			break;
		case ZTEXFMT_A8B8G8R8:
			DdsHeader.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			// DdsHeader.ddpfPixelFormat.dwFourCC             = D3DFMT_A8B8G8R8; /* 32 */
			DdsHeader.ddpfPixelFormat.dwRGBBitCount = 32;
			DdsHeader.ddpfPixelFormat.dwRBitMask = 0x000000FF;
			DdsHeader.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
			DdsHeader.ddpfPixelFormat.dwBBitMask = 0x00FF0000;
			DdsHeader.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
			break;
		case ZTEXFMT_A8R8G8B8:
			DdsHeader.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			// DdsHeader.ddpfPixelFormat.dwFourCC             = D3DFMT_A8R8G8B8; /* 21 */
			DdsHeader.ddpfPixelFormat.dwRGBBitCount = 32;
			DdsHeader.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
			DdsHeader.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
			DdsHeader.ddpfPixelFormat.dwBBitMask = 0x000000FF;
			DdsHeader.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
			break;
		case ZTEXFMT_B8G8R8:
			DdsHeader.ddpfPixelFormat.dwFlags = DDPF_RGB;
			DdsHeader.ddpfPixelFormat.dwRGBBitCount = 24;
			DdsHeader.ddpfPixelFormat.dwRBitMask = 0x000000FF;
			DdsHeader.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
			DdsHeader.ddpfPixelFormat.dwBBitMask = 0x00FF0000;
			break;
		case ZTEXFMT_R8G8B8:
			DdsHeader.ddpfPixelFormat.dwFlags = DDPF_RGB;
			// DdsHeader.ddpfPixelFormat.dwFourCC         = D3DFMT_R8G8B8; /* 20 */
			DdsHeader.ddpfPixelFormat.dwRGBBitCount = 24;
			DdsHeader.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
			DdsHeader.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
			DdsHeader.ddpfPixelFormat.dwBBitMask = 0x000000FF;
			break;
		case ZTEXFMT_A4R4G4B4:
			DdsHeader.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			// DdsHeader.ddpfPixelFormat.dwFourCC             = D3DFMT_A4R4G4B4; /* 26 */
			DdsHeader.ddpfPixelFormat.dwRGBBitCount = 16;
			DdsHeader.ddpfPixelFormat.dwRBitMask = 0x00000F00;
			DdsHeader.ddpfPixelFormat.dwGBitMask = 0x000000F0;
			DdsHeader.ddpfPixelFormat.dwBBitMask = 0x0000000F;
			DdsHeader.ddpfPixelFormat.dwRGBAlphaBitMask = 0x0000F000;
			break;
		case ZTEXFMT_A1R5G5B5:
			DdsHeader.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			// DdsHeader.ddpfPixelFormat.dwFourCC             = D3DFMT_A1R5G5B5; /* 25 */
			DdsHeader.ddpfPixelFormat.dwRGBBitCount = 16;
			DdsHeader.ddpfPixelFormat.dwRBitMask = 0x00007C00;
			DdsHeader.ddpfPixelFormat.dwGBitMask = 0x000003E0;
			DdsHeader.ddpfPixelFormat.dwBBitMask = 0x0000001F;
			DdsHeader.ddpfPixelFormat.dwRGBAlphaBitMask = 0x00008000;
			break;
		case ZTEXFMT_R5G6B5:
			DdsHeader.ddpfPixelFormat.dwFlags = DDPF_RGB;
			// DdsHeader.ddpfPixelFormat.dwFourCC         = D3DFMT_R5G6B5; /* 23 */
			DdsHeader.ddpfPixelFormat.dwRGBBitCount = 16;
			DdsHeader.ddpfPixelFormat.dwRBitMask = 0x0000F800;
			DdsHeader.ddpfPixelFormat.dwGBitMask = 0x000007E0;
			DdsHeader.ddpfPixelFormat.dwBBitMask = 0x0000001F;
			break;
		case ZTEXFMT_P8:
			DdsHeader.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8;
			// DdsHeader.ddpfPixelFormat.dwFourCC = D3DFMT_P8; /* 41 */
			break;
		case ZTEXFMT_DXT1:
			DdsHeader.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			DdsHeader.ddpfPixelFormat.dwFourCC = MAKEFOURCC('D', 'X', 'T', '1');
			break;
		case ZTEXFMT_DXT2:
			DdsHeader.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			DdsHeader.ddpfPixelFormat.dwFourCC = MAKEFOURCC('D', 'X', 'T', '2');
			break;
		case ZTEXFMT_DXT3:
			DdsHeader.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			DdsHeader.ddpfPixelFormat.dwFourCC = MAKEFOURCC('D', 'X', 'T', '3');
			break;
		case ZTEXFMT_DXT4:
			DdsHeader.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			DdsHeader.ddpfPixelFormat.dwFourCC = MAKEFOURCC('D', 'X', 'T', '4');
			break;
		case ZTEXFMT_DXT5:
			DdsHeader.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			DdsHeader.ddpfPixelFormat.dwFourCC = MAKEFOURCC('D', 'X', 'T', '5');
			break;
		}
		if(optionForceARGB)
		{
			switch(ZTexHeader.TexInfo.Format)
			{
			case ZTEXFMT_B8G8R8A8:
			case ZTEXFMT_R8G8B8A8:
			case ZTEXFMT_A8B8G8R8:
				DdsHeader.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
				DdsHeader.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
				DdsHeader.ddpfPixelFormat.dwBBitMask = 0x000000FF;
				DdsHeader.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
				break;
			case ZTEXFMT_B8G8R8:
				DdsHeader.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
				DdsHeader.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
				DdsHeader.ddpfPixelFormat.dwBBitMask = 0x000000FF;
				break;
			default:
				break;
			}
		}

		if(!writeVectorData(&DdsHeader, sizeof(DdsHeader), ddsData))
		{
			return ZTEX2DDS_ERROR_WRITE;
		}

		/* Palette */
		if(ZTEXFMT_P8 == ZTexHeader.TexInfo.Format)
		{
			for(i = 0; i < ZTEX_PAL_ENTRIES; i++)
			{
				palentry.rgbReserved = 0x00;
				if(!readVectorData(&palentry, BytesRead, sizeof(palentry), ztexData))
				{
					return ZTEX2DDS_ERROR_READ;
				}

				BytesRead += sizeof(palentry);

				if(!writeVectorData(&palentry, sizeof(ozRGBQUAD), ddsData))
				{
					return ZTEX2DDS_ERROR_WRITE;
				}
			}
		}
		/* Mipmaps */
		MipmapCount = std::max(1u, ZTexHeader.TexInfo.MipMaps);
		BufferSize = 0;
		for(MipmapLevel = 0; MipmapLevel < MipmapCount; MipmapLevel++)
			BufferSize += GetMipmapSize(ZTexHeader.TexInfo.Format,
				ZTexHeader.TexInfo.Width, ZTexHeader.TexInfo.Height, MipmapLevel);

		Buffer = new uint8_t[BufferSize];
		if(!Buffer)
		{
			return ZTEX2DDS_ERROR_READ;
		}

		if(!readVectorData(Buffer, BytesRead, BufferSize, ztexData))
		{
			return ZTEX2DDS_ERROR_READ;
		}
		BytesRead += BufferSize;

		Mipmap = Buffer + BufferSize;
		for(MipmapLevel = 0; MipmapLevel < MipmapCount; MipmapLevel++)
		{
			MipmapSize = GetMipmapSize(ZTexHeader.TexInfo.Format,
				ZTexHeader.TexInfo.Width, ZTexHeader.TexInfo.Height, MipmapLevel);
			Mipmap -= MipmapSize;
			if(optionForceARGB)
			{
				Pixel32 = (ozRGBQUAD *)Mipmap;
				Pixel24 = (ozRGBTRIPLE *)Mipmap;
				switch(ZTexHeader.TexInfo.Format)
				{
				case ZTEXFMT_B8G8R8A8:
					for(i = 0; i < (int)MipmapSize / 4; i++)
					{
						ColorTemp = Pixel32->rgbReserved;
						Pixel32->rgbReserved = Pixel32->rgbBlue;
						Pixel32->rgbBlue = ColorTemp;
						ColorTemp = Pixel32->rgbRed;
						Pixel32->rgbRed = Pixel32->rgbGreen;
						Pixel32->rgbGreen = ColorTemp;
						Pixel32++;
					}
					break;
				case ZTEXFMT_R8G8B8A8:
					for(i = 0; i < (int)MipmapSize / 4; i++)
					{
						ColorTemp = Pixel32->rgbBlue;
						Pixel32->rgbBlue = Pixel32->rgbGreen;
						Pixel32->rgbGreen = Pixel32->rgbRed;
						Pixel32->rgbRed = Pixel32->rgbReserved;
						Pixel32->rgbReserved = ColorTemp;
						Pixel32++;
					}
					break;
				case ZTEXFMT_A8B8G8R8:
					for(i = 0; i < (int)MipmapSize / 4; i++)
					{
						ColorTemp = Pixel32->rgbBlue;
						Pixel32->rgbBlue = Pixel32->rgbRed;
						Pixel32->rgbRed = ColorTemp;
						Pixel32++;
					}
					break;
				case ZTEXFMT_B8G8R8:
					for(i = 0; i < (int)MipmapSize / 3; i++)
					{
						ColorTemp = Pixel24->rgbtBlue;
						Pixel24->rgbtBlue = Pixel24->rgbtRed;
						Pixel24->rgbtRed = ColorTemp;
						Pixel24++;
					}
					break;
				default:
					break;
				}
			}

			if(!writeVectorData(Mipmap, MipmapSize, ddsData))
			{
				delete[] Buffer;
				return ZTEX2DDS_ERROR_WRITE;
			}
		}
		delete[] Buffer;

		return ZTEX2DDS_ERROR_NONE;
	}

	uint32_t ComputeSizeInBytes(int mip, int resolutionX, int resolutionY, bool dxt1)
	{
		int px = (int)std::max(1.0f, (float)floor(resolutionX / pow(2.0f, mip)));
		int py = (int)std::max(1.0f, (float)floor(resolutionY / pow(2.0f, mip)));
		//int px = TextureSize.x;
		//int py = TextureSize.y;

		// compute the storage requirements
		int blockcount = ( ( px + 3 )/4 ) * ( ( py + 3 )/4 );
		int blocksize = dxt1 ? 8 : 16;
		return blockcount*blocksize;	
	}

	/**
	 * @param ddsData Loaded dds
	 * @return Pointer to the surface info of the given dds
	 */
	DDSURFACEDESC2 getSurfaceDesc( const std::vector<uint8_t>& ddsData)
	{
		DDSURFACEDESC2 desc;

		memcpy(&desc, &ddsData[sizeof(uint32_t)], sizeof(DDSURFACEDESC2));

		return desc;
	}

	/**
	 * @brief Convert dds to RGBA8
	 */
	void convertDDSToRGBA8(const std::vector<uint8_t>& ddsData, std::vector<uint8_t>& rgba8Data, int mip)
	{
		size_t seek=0;
		seek += sizeof(uint32_t); // Skip magic number
		const DDSURFACEDESC2* desc = reinterpret_cast<const DDSURFACEDESC2*>(&ddsData[seek]);
		seek += sizeof(DDSURFACEDESC2);

		mip = std::min((int)mip, (int)desc->dwMipMapCount);

		int dxtlvl = squish::kDxt1;
		switch(desc->ddpfPixelFormat.dwFourCC)
		{
		case MAKEFOURCC('D', 'X', 'T', '1'):
			dxtlvl = squish::kDxt1;
			break;

		case MAKEFOURCC('D', 'X', 'T', '3'):
			dxtlvl = squish::kDxt3;
			break;

		case MAKEFOURCC('D', 'X', 'T', '5'):
			dxtlvl = squish::kDxt5;
			break;
		}

		for(int i = 0; i < mip; i++)
		{
			seek += ComputeSizeInBytes(i, desc->dwWidth, desc->dwHeight, dxtlvl == squish::kDxt1);
		}

		int px = (int)std::max(1.0f, (float)floor(desc->dwWidth / pow(2.0f, mip)));
		int py = (int)std::max(1.0f, (float)floor(desc->dwHeight / pow(2.0f, mip)));
		rgba8Data.resize(px * py* sizeof(uint32_t));

		squish::DecompressImage(rgba8Data.data(), px, py, &ddsData[seek], dxtlvl);
	}
}
/* THE END */
