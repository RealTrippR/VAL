/*
Copyright © 2025 Tripp Robins

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the “Software”), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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