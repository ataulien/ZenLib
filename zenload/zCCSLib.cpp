#include "zCCSLib.h"
#include "zenParser.h"
#include "parserImpl.h"
#include "zenParserPropRead.h"
#include <vdfs/fileIndex.h>
#include <utils/logger.h>
#include <assert.h>

using namespace ZenLoad;

zCCSLib::zCCSLib(const std::string& fileName, const VDFS::FileIndex& fileIndex)
{
    std::vector<uint8_t> data;
    fileIndex.getFileData(fileName, data);

    if(data.empty())
        return; // TODO: Throw an exception or something

    try
    {
        // Create parser from memory
        // FIXME: There is an internal copy of the data here. Optimize!
        ZenLoad::ZenParser parser(data.data(), data.size());

        readObjectData(parser);
    }
    catch(std::exception &e)
    {
        LogError() << e.what();
        return;
    }
}


zCCSLib::zCCSLib(const std::string& file)
{
    try
    {
        // Create parser from memory
        // FIXME: There is an internal copy of the data here. Optimize!
        ZenLoad::ZenParser parser(file);

        readObjectData(parser);
    }
    catch(std::exception &e)
    {
        LogError() << e.what();
        return;
    }
}


void zCCSLib::readObjectData(ZenParser& parser)
{
    zCCSLibData& info = m_Data;

    parser.readHeader();

    ZenParser::ChunkHeader libHeader;
    parser.readChunkStart(libHeader);

    assert(libHeader.classname == "zCCSLib");

    uint32_t numItems;
    parser.getImpl()->readEntry("NumOfItems", &numItems, sizeof(numItems), ParserImpl::ZVT_INT);

    //LogInfo() << "Reading " << numItems << " blocks";

    for(uint32_t i = 0;i<numItems;i++)
    {
        ZenParser::ChunkHeader blockHeader;
        parser.readChunkStart(blockHeader);

        zCCSBlockData blk;
        uint32_t numBlocks;
        float subBlock0;
        ReadObjectProperties(parser, blk.properties,
                             Prop("blockName", blk.blockName),
                             Prop("numOfBlocks", numBlocks),
                             Prop("subBlock0", subBlock0));

        // Haven't seen different values for these
        assert(numBlocks == 1);
        assert(subBlock0 == 0.0f);

        // Read the single atomic block
        {
            ZenParser::ChunkHeader atomicHeader;

            parser.readChunkStart(atomicHeader);

            // Read event-message of atomic block
            {
                ZenParser::ChunkHeader messageHeader;
                parser.readChunkStart(messageHeader);

                ReadObjectProperties(parser, blk.atomicBlockData.properties,
                                     Prop("subType", blk.atomicBlockData.command.subType),
                                          Prop("text", blk.atomicBlockData.command.text),
                                          Prop("name", blk.atomicBlockData.command.name));

                //LogInfo() << "Read message: " << blk.atomicBlockData.command.name;

                parser.readChunkEnd();
            }

            parser.readChunkEnd();
        }

        info.blocks.push_back(blk.atomicBlockData);
        m_MessagesByName[blk.blockName] = info.blocks.size()-1;
        //LogInfo() << blk.blockName;

    }

}

const oCMsgConversationData& zCCSLib::getMessageByName(const std::string& name)
{
    size_t idx = m_MessagesByName[name];
    return m_Data.blocks[idx].command;
}

