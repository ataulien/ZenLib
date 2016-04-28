#pragma once

#include <inttypes.h>
#include <fstream>

#define ZEN_SCALE_FACTOR (1.0f / 100.0f)

namespace ZenLoad
{
    class Chunk;

	class Export
	{
	public:
		Export(const std::string &dirName);
		virtual ~Export(){}
        virtual void exportVobTree(Chunk *pVob);
        virtual void exportWayNet(Chunk *pVob, std::ostream &stream);
        virtual void exportMaterials(Chunk *pVob, std::ostream &stream);
        virtual void exportWorld(Chunk *pVob, std::ostream &stream);
        virtual void exportVobs(Chunk *pVob, std::ostream &stream);

    protected:
        std::string m_DirectoryName;
    };
}
