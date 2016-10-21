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
	 * @param ddsData Loaded dds
	 * @return surface info of the given dds
	 */
	DDSURFACEDESC2 getSurfaceDesc( const std::vector<uint8_t>& ddsData);
}
