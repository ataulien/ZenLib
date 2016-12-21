//
// Created by desktop on 21.10.16.
//

#include "zCFont.h"
#include "zenParser.h"
#include <vdfs/fileIndex.h>
#include <utils/logger.h>
#include <algorithm>

using namespace ZenLoad;

zCFont::zCFont(const std::string& fileName, const VDFS::FileIndex& fileIndex)
{
    std::string fntFile = fileName;
    std::transform(fntFile.begin(), fntFile.end(), fntFile.begin(), ::tolower);

    if(fntFile.find(".tga") != std::string::npos)
        fntFile = fntFile.substr(0, fntFile.size() - 4) + ".FNT";
    else if(fntFile.find(".fnt") == std::string::npos)
        fntFile += ".FNT";

    std::vector<uint8_t> data;
    fileIndex.getFileData(fileName, data);

    if(data.empty())
        return; // TODO: Throw an exception or something

    parseFNTData(data);
}

zCFont::~zCFont()
{

}

bool zCFont::parseFNTData(const std::vector<uint8_t>& fntData)
{
    try
    {
        // Create parser from memory
        // FIXME: There is an internal copy of the data here. Optimize!
        ZenLoad::ZenParser parser(fntData.data(), fntData.size());

        /**
         * FNT-format is pretty simple:
         * [string]: version\n
         * [string]: name\n
         * [uint32_t]: height
         * [uint32_t]: magicNumber? (MUST be always 0xFF)
         * [uint8_t]: width[FONT_NUM_MAX_LETTERS]
         * [float2]:  fontUV1[FONT_NUM_MAX_LETTERS]
         * {float2]:  fontUV2[FONT_NUM_MAX_LETTERS]
         */

        std::string version = parser.readLine();

        // Only version 1 is used by gothic
        if(version != "1")
        {
            LogError() << "Unknown font-version: " << version;
            return false;
        }

        std::string name = parser.readLine();

        uint32_t height = parser.readBinaryDWord();
        uint32_t magic = parser.readBinaryDWord();

        if(magic != 256)
        {
            LogError() << "Invalid font-file! Magic: " << magic;
            return false;
        }

        // Checks are through, directly write to font-info now

        parser.readBinaryRaw(m_Info.glyphWidth, sizeof(m_Info.glyphWidth));
        parser.readBinaryRaw(m_Info.fontUV1, sizeof(m_Info.fontUV1));
        parser.readBinaryRaw(m_Info.fontUV2, sizeof(m_Info.fontUV2));

        // Plug the other information in
        m_Info.fontName = name;
        m_Info.fontHeight = height;
    }
    catch(std::exception &e)
    {
        LogError() << e.what();
        return false;
    }

    return true;
}
