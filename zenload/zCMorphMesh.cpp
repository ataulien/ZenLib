#include "zCMorphMesh.h"
#include "zenParser.h"
#include "utils/logger.h"
#include "zTypes.h"
#include <string>
#include "vdfs/fileIndex.h"
#include "zCProgMeshProto.h"
#include "utils/alignment.h"

using namespace ZenLoad;

static const uint16_t MMID_MMB_HEADER = 0xE020;
static const uint16_t MMID_MMB_ANILIST = 0xE030;

zCMorphMesh::zCMorphMesh(const std::string& fileName, const VDFS::FileIndex& fileIndex)
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

/**
* @brief Reads the mesh-object from the given binary stream
*/
void zCMorphMesh::readObjectData(ZenParser& parser)
{
    // Information about the whole file we are reading here
    BinaryFileInfo fileInfo;

    // Information about a single chunk
    BinaryChunkInfo chunkInfo;


    // Read chunks until we left the virtual binary file or got to the end-chunk
    // Each chunk starts with a header (BinaryChunkInfo) which gives information
    // about what to do and how long the chunk is
    bool doneReadingChunks = false;
    while(!doneReadingChunks)
    {
        // Read chunk header and calculate position of next chunk
        parser.readStructure(chunkInfo);

        size_t chunkEnd = parser.getSeek() + chunkInfo.length;

        switch(chunkInfo.id)
        {
            case MMID_MMB_HEADER:
            {
                uint32_t version = parser.readBinaryDWord();

                std::string morphProtoName = parser.readLine(true);
                //LogInfo() << "MorphProtoName: " << morphProtoName;

                // Read source-mesh
                m_Mesh.readObjectData(parser);

                // TODO: use these
                std::vector<ZMath::float3> morphPositions(m_Mesh.getVertices().size());
                parser.readBinaryRaw(morphPositions.data(), sizeof(ZMath::float3) * m_Mesh.getVertices().size());
            }
                break;

            case MMID_MMB_ANILIST:
                doneReadingChunks = true;
                parser.setSeek(chunkEnd);
                break;

                // Morphmeshes don't have a real end-tag. The anilist is the last one, however

            default:
                parser.setSeek(chunkEnd); // Skip chunk

        }
    }
}
