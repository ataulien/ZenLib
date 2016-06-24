#pragma once
#include <vector>
#include "utils/mathlib.h"
#include "zTypes.h"
#include "zCProgMeshProto.h"

namespace VDFS
{
    class FileIndex;
}

namespace ZenLoad
{
    class ZenParser;
    class zCMorphMesh
    {
    public:

        zCMorphMesh(){}

        /**
		 * @brief Loads the mesh from the given VDF-Archive
		 */
        zCMorphMesh(const std::string& fileName, const VDFS::FileIndex& fileIndex);

        /**
         * @brief Reads the mesh-object from the given binary stream
         * @param fromZen Whether this mesh is supposed to be read from a zenfile. In this case, information about the binary chunk is also read.
         */
        void readObjectData(ZenParser& parser);

        /**
         * @return Internal zCProgMeshProto of this soft skin. The soft-skin only displaces the vertices found in the ProgMesh.
         */
        const zCProgMeshProto& getMesh() const { return m_Mesh; }
    private:
        /**
         * @brief Internal zCProgMeshProto of this soft skin. The soft-skin only displaces the vertices found in the ProgMesh.
         */
        zCProgMeshProto m_Mesh;
    };
}
