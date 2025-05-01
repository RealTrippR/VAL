#ifndef VAL_MODEL_LOADER_HPP
#define VAL_MODEL_LOADER_HPP


#include <VAL/lib/system/system_utils.hpp>

#include <filesystem>
#include <vector>

#include <VAL/lib/meshes&vertices/vertex3Dtextured.hpp>
#include <VAL/lib/meshes&vertices/meshTextured.hpp>
#include <VAL/lib/meshes&vertices/vertex3Dsimple.hpp>
#include <VAL/lib/meshes&vertices/meshSimple.hpp>


#ifndef VAL_TINY_OBJ_H
#define VAL_TINY_OBJ_H
#include <ExternalLibraries/tiny_obj_loader.h>
#endif // !VAL_TINY_OBJ_H



namespace val {

    namespace fs = std::filesystem;

    void loadModelFromDiskAsUnifiedMesh(fs::path modelPath, std::vector<val::vertex3D>& verticesOut, std::vector<uint32_t>& indicesOut,
        tinyobj::attrib_t* meshAttribsOut, bool deduplicateVertices = true);

    void loadModelFromDisk(fs::path modelPath, std::vector<std::vector<val::vertex3D>>& verticesOut, std::vector<std::vector<uint32_t>>& indicesOut,
        tinyobj::attrib_t* meshAttribsOut, bool deduplicateVertices = true);

    void loadModelFromDiskAsUnifiedMesh(fs::path modelPath, std::vector<val::vertex3Dsimple>& verticesOut, std::vector<uint32_t>& indicesOut,
        tinyobj::attrib_t* meshAttribsOut, bool deduplicateVertices = true);

    void loadModelFromDisk(fs::path modelPath, std::vector<std::vector<val::vertex3Dsimple>>& verticesOut, std::vector<std::vector<uint32_t>>& indicesOut,
        tinyobj::attrib_t* meshAttribsOut, bool deduplicateVertices = true);
}

#endif // !VAL_MODEL_LOADER_HPP