/*
Copyright � 2025 Tripp Robins

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the �Software�), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <VAL/lib/meshes&vertices/loadModels.hpp>

namespace val {

    namespace fs = std::filesystem;

    void loadModelFromDiskAsUnifiedMesh(fs::path modelPath, std::vector<val::vertex3D>& verticesOut, std::vector<uint32_t>& indicesOut,
        tinyobj::attrib_t* meshAttribsOut, bool deduplicateVertices /*Default: True*/)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.string().c_str())) {
            throw std::runtime_error(warn + err);
        }

        if (deduplicateVertices) {
            std::unordered_map<val::vertex3D, uint32_t> uniqueVertices{};

            for (const auto& shape : shapes) {
                for (const auto& index : shape.mesh.indices) {
                    val::vertex3D vertex{};

                    vertex.pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.texCoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };

                    vertex.color = { 1.0f, 1.0f, 1.0f };

                    if (uniqueVertices.count(vertex) == 0) {
                        uniqueVertices[vertex] = static_cast<uint32_t>(verticesOut.size());
                        verticesOut.push_back(vertex);
                    }

                    indicesOut.push_back(uniqueVertices[vertex]);
                }
            }
        }
        else {
            for (const auto& shape : shapes) {
                for (const auto& index : shape.mesh.indices) {
                    val::vertex3D vertex{};

                    vertex.pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.texCoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };

                    vertex.color = { 1.0f, 1.0f, 1.0f };

                    indicesOut.push_back(verticesOut.size());
                    verticesOut.push_back(vertex);
                }
            }
        }

        if (meshAttribsOut) {
            *meshAttribsOut = attrib;
        }
    }

    void loadModelFromDisk(fs::path modelPath, std::vector<std::vector<val::vertex3D>>& verticesOut, std::vector<std::vector<uint32_t>>& indicesOut,
        tinyobj::attrib_t* meshAttribsOut, bool deduplicateVertices /*DEfault: true*/)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.string().c_str())) {
            throw std::runtime_error(warn + err);
        }

        verticesOut.resize(shapes.size());
        indicesOut.resize(shapes.size());

        if (deduplicateVertices) {
            for (size_t shapeIdx = 0; shapeIdx < shapes.size(); ++shapeIdx) {
                const auto shape = shapes[shapeIdx];
                for (const auto& index : shape.mesh.indices) {
                    vertex3D vertex{};

                    vertex.pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.texCoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };

                    vertex.color = { 1.0f, 1.0f, 1.0f };

                    indicesOut[shapeIdx].push_back(verticesOut.size());
                    verticesOut[shapeIdx].push_back(vertex);
                }
            }
        }
        else {
            for (size_t shapeIdx = 0; shapeIdx < shapes.size(); ++shapeIdx) {
                \
                    std::unordered_map<vertex3D, uint32_t> uniqueVertices{};

                const auto shape = shapes[shapeIdx];
                for (const auto& index : shape.mesh.indices) {
                    vertex3D vertex{};

                    vertex.pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.texCoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };

                    if (uniqueVertices.count(vertex) == 0) {
                        uniqueVertices[vertex] = static_cast<uint32_t>(verticesOut.size());
                        verticesOut[shapeIdx].push_back(vertex);
                    }

                    indicesOut[shapeIdx].push_back(uniqueVertices[vertex]);
                }
            }
        }

        if (meshAttribsOut) {
            *meshAttribsOut = attrib;
        }
    }

    void loadModelFromDiskAsUnifiedMesh(fs::path modelPath, std::vector<val::vertex3Dsimple>& verticesOut, std::vector<uint32_t>& indicesOut,
        tinyobj::attrib_t* meshAttribsOut, bool deduplicateVertices /*Default: True*/)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.string().c_str())) {
            throw std::runtime_error(warn + err);
        }

        if (deduplicateVertices) {
            std::unordered_map<val::vertex3Dsimple, uint32_t> uniqueVertices{};

            for (const auto& shape : shapes) {
                for (const auto& index : shape.mesh.indices) {
                    val::vertex3Dsimple vertex{};

                    vertex.pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };


                    vertex.color = { 1.0f, 1.0f, 1.0f };

                    if (uniqueVertices.count(vertex) == 0) {
                        uniqueVertices[vertex] = static_cast<uint32_t>(verticesOut.size());
                        verticesOut.push_back(vertex);
                    }

                    indicesOut.push_back(uniqueVertices[vertex]);
                }
            }
        }
        else {
            for (const auto& shape : shapes) {
                for (const auto& index : shape.mesh.indices) {
                    val::vertex3Dsimple vertex{};

                    vertex.pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.color = { 1.0f, 1.0f, 1.0f };

                    indicesOut.push_back(verticesOut.size());
                    verticesOut.push_back(vertex);
                }
            }
        }
        if (meshAttribsOut) {
            *meshAttribsOut = attrib;
        }
    }

    void loadModelFromDisk(fs::path modelPath, std::vector<std::vector<val::vertex3Dsimple>>& verticesOut, std::vector<std::vector<uint32_t>>& indicesOut,
        tinyobj::attrib_t* meshAttribsOut, bool deduplicateVertices /*DEfault: true*/)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.string().c_str())) {
            throw std::runtime_error(warn + err);
        }

        verticesOut.resize(shapes.size());
        indicesOut.resize(shapes.size());

        if (deduplicateVertices) {
            for (size_t shapeIdx = 0; shapeIdx < shapes.size(); ++shapeIdx) {
                const auto shape = shapes[shapeIdx];
                for (const auto& index : shape.mesh.indices) {
                    vertex3Dsimple vertex{};

                    vertex.pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.color = { 1.0f, 1.0f, 1.0f };

                    indicesOut[shapeIdx].push_back(verticesOut.size());
                    verticesOut[shapeIdx].push_back(vertex);
                }
            }
        }
        else {
            for (size_t shapeIdx = 0; shapeIdx < shapes.size(); ++shapeIdx) {
                
                std::unordered_map<vertex3Dsimple, uint32_t> uniqueVertices{};

                const auto shape = shapes[shapeIdx];
                for (const auto& index : shape.mesh.indices) {
                    vertex3Dsimple vertex{};

                    vertex.pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    if (uniqueVertices.count(vertex) == 0) {
                        uniqueVertices[vertex] = static_cast<uint32_t>(verticesOut.size());
                        verticesOut[shapeIdx].push_back(vertex);
                    }

                    indicesOut[shapeIdx].push_back(uniqueVertices[vertex]);
                }
            }
        }
        if (meshAttribsOut) {
            *meshAttribsOut = attrib;
        }
    }
}