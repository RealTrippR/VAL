#ifndef VAL_MODEL_LOADER_HPP
#define VAL_MODEL_LOADER_HPP

#include <VAL/lib/meshes&vertices/vertex3Dtextured.hpp>
#include <VAL/lib/system/system_utils.hpp>
#include <ExternalLibraries/tiny_obj_loader.h>
#include <filesystem>
#include <vector>

namespace val {

    namespace fs = std::filesystem;

    void loadModelFromDiskAsUnifiedMesh(fs::path modelPath, std::vector<val::vertex3D>& verticesOut, std::vector<uint32_t>& indicesOut,
        tinyobj::attrib_t& meshAttribsOut, bool deduplicateVertices = true);

    void loadModelFromDisk(fs::path modelPath, std::vector<std::vector<val::vertex3D>>& verticesOut, std::vector<std::vector<uint32_t>>& indicesOut,
        tinyobj::attrib_t& meshAttribsOut, bool deduplicateVertices = true);
}

#endif // !VAL_MODEL_LOADER_HPP