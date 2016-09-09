#pragma once
#include <zenload/zTypes.h>

namespace Utils
{
    /**
     * Exports the given packed-mesh structure into a wavefront-OBJ-file
     * @param mesh Mesh to export
     * @param file Target OBJ-File
     * @param name Name of the object to export
     * @return Whether the export succeeded
     */
    inline bool exportPackedMeshToObj(ZenLoad::PackedMesh& mesh, const std::string& file, const std::string& name="Object")
    {
        FILE* f = fopen(file.c_str(), "w");

        if(!f)
            return false;

        fputs("# File exported by ZenLib\n", f);
        fputs(("o " + name + "\n").c_str(), f);

        // Write positions
        for(auto& v : mesh.vertices)
        {
            fputs(("v " + std::to_string(v.Position.x) + " "
                      + std::to_string(v.Position.y) + " "
                      + std::to_string(v.Position.z) + "\n").c_str(), f);
        }

        // Write texture coords
        for(auto& v : mesh.vertices)
        {
            fputs(("v " + std::to_string(v.TexCoord.x) + " "
                      + std::to_string(v.TexCoord.y) + "\n").c_str(), f);
        }

        // Write normals
        for(auto& v : mesh.vertices)
        {
            fputs(("v " + std::to_string(v.Normal.x) + " "
                      + std::to_string(v.Normal.y) + " "
                      + std::to_string(v.Normal.z) + "\n").c_str(), f);
        }

        
        fputs("s off\n", f);

        // Write faces
        for(auto& s : mesh.subMeshes)
        {
            fputs(("g " + s.material.matName + "\n").c_str(), f);
            fputs(("usemtl " + s.material.matName + "\n").c_str(), f);
            
            for(size_t i=0;i<s.indices.size();i+=3)
            {
                uint32_t idx[] = {s.indices[i], s.indices[i+1], s.indices[i+2]};
                std::string f1 = std::to_string(idx[0]+1) + "/" + std::to_string(idx[0]+1) + "/" + std::to_string(idx[0]+1);
                std::string f2 = std::to_string(idx[1]+1) + "/" + std::to_string(idx[1]+1) + "/" + std::to_string(idx[1]+1);
                std::string f3 = std::to_string(idx[2]+1) + "/" + std::to_string(idx[2]+1) + "/" + std::to_string(idx[2]+1);

                fputs(("f " + f1 + " " + f2 + " " + f3 + "\n").c_str(), f);
            }
        }

        // Write mtl
        FILE* mf = fopen((file + ".mtl").c_str(), "w");
        
        if(!mf)
            return false;
        
        for(auto& s : mesh.subMeshes)
        {
            fputs(("newmtl " + s.material.matName + "\n").c_str(), mf);
        
            fputs(("map_Kd " + s.material.texture + "\n").c_str(), mf);

            
            ZMath::float4 color; color.fromABGR8(s.material.color);
            
            fputs(("Kd " + std::to_string(color.x) + " " + std::to_string(color.y) + " " + std::to_string(color.z) + "\n").c_str(), mf);
            fputs(("Ka " + std::to_string(color.x) + " " + std::to_string(color.y) + " " + std::to_string(color.z) + "\n").c_str(), mf);
            
            
            fputs("\n", mf);
        }
        
        
        fclose(mf);
        
        return true;
    }
}
