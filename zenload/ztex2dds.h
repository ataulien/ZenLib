#pragma once
#include <vector>
#include <inttypes.h>
#include "ztex.h"

namespace ZenLoad
{
	/**
	* @brief Modified ZTEX to DDS conversion
	*/
	int convertZTEX2DDS( const std::vector<uint8_t>& ztexData, std::vector<uint8_t>& ddsData, bool optionForceARGB = false, int* pOutWidth = nullptr, int* pOutHeight = nullptr);

	/**
	 * @brief Convert dds to RGBA8
	 */
	void convertDDSToRGBA8( const std::vector<uint8_t>& ddsData, std::vector<uint8_t>& rgba8Data, int mip=0);

	/**
	 * @return Total offset (including header) of where the given mip-level starts inside ddsData
	 */
	size_t getMipFileOffsetFromDDS(const std::vector<uint8_t>& ddsData, int mip);

	enum class DXTLevel
	{
		DXT1,
		DXT3,
		DXT5,
		Unknown,
	};

	/**
	 * @return The DXT compression level of the given DDS file
	 */
	DXTLevel getDXTLevelFromDDS(const std::vector<uint8_t>& ddsData);

	/**
	 * @param ddsData Loaded dds
	 * @return surface info of the given dds
	 */
	DDSURFACEDESC2 getSurfaceDesc( const std::vector<uint8_t>& ddsData);
}
