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

#ifndef VAL_MESH_HPP
#define VAL_MESH_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/ext/gpu_vector.hpp>

#include <VAL/lib/meshes&vertices/fbxScene.hpp>
#include <VAL/lib/meshes&vertices/objScene.hpp>

#include <inttypes.h>
#include <stdlib.h>

#include <unordered_map>
#include <VAL/lib/ext/ubfx.h>

namespace val
{
	struct TextureUVBinding 
	{
		Texture2D* texture = NULL;

		// the UV / layout index which corresponds
		// to a (layout = n) in a shader.
		uint32_t layoutIdx;
	};

	template <typename VT/*vertex type*/, uint16_t NTextures>
	class Mesh
	{
	public:
		gpu_vector<VT> vertices;
		gpu_vector<uint32_t> indices;

		std::array<TextureUVBinding, NTextures> textureBindings;
	public:

		~Mesh()
		{
#ifndef NDEBUG
			if (indices.empty() == false || vertices.empty() == false)
			{
				dbg::printError("Mesh @ %p has not been properly destroyed.");
				throw std::runtime_error("Mesh has not been properly destroyed.");
			}
#endif // !NDEBUG
		}
	public:
		void destroy(ValProc& proc)
		{
			vertices.destroy(proc);
			indices.destroy(proc);
		}

		VAL_RETURN_CODE importFromScene(ValProc& proc, ufbx_scene* scene, const uint32_t meshIndex, bool deduplicateVertices = true);

		VAL_RETURN_CODE importFromScene(ValProc& proc, ObjScene& scene, const uint32_t meshIndex, bool deduplicateVertices = true);

		void setTexture(Texture2D& texture, const uint32_t bindingIndex, const uint32_t location)
		{
#ifndef NDEBUG
			if (bindingIndex >= NTextures) {
				dbg::printError("Mesh::setTexture: Attempted to set texture at an invalid binding index of %lu for Mesh @ %p", bindingIndex, this);
			}
#endif // !NDEBUG
			textureBindings[bindingIndex].layoutIdx = 0u;// = { &texture, bindingIndex };
		}

		const Texture2D* getTexture(uint8_t bindingIndex) const
		{
			return textureBindings[bindingIndex].texture;
		}

		const TextureUVBinding& getTextureBinding(uint8_t bindingIndex) {
			return textureBindings[bindingIndex];
		}

	private:



	};


	// import from scene, FBX
	template <typename VT/*vertex type*/, uint16_t NTextures>
	VAL_RETURN_CODE Mesh<VT, NTextures>::importFromScene(ValProc& proc, ufbx_scene* scene, const uint32_t meshIndex, bool deduplicateVertices)
	{
		vertices.destroy(proc);
		indices.destroy(proc);

		indices.setUsage(proc, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		vertices.setUsage(proc, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

		size_t totalIndexCount = 0u;
		for (size_t ni = 0; ni < scene->nodes.count; ni++) {
			ufbx_node* node = scene->nodes.data[ni];
			if (node->is_root) continue;
			if (node->mesh) {
				totalIndexCount += node->mesh->vertices.count;
			}
		}
		if (totalIndexCount > UINT32_MAX) {
			dbg::printWarning("mesh::importFromScene: Failed to load mesh, vertex count exceeds UINT32_MAX");
			return VAL_FAILURE;
		}
		if (totalIndexCount == 0) {
			return VAL_SUCCESS; // there no mesh to load
		}



		uint32_t lastIndex = 0u;
		std::unordered_map<VT, uint32_t> uniqueVertices{};

		for (size_t ni = 0; ni < scene->nodes.count; ni++)
		{
			ufbx_node* node = scene->nodes.data[ni];
			if (node->is_root) continue;

			if (node->mesh)
			{
				ufbx_mesh* mesh = node->mesh;

				// load vertices and indices from triangulated faces.
				for (size_t fi = 0; fi < mesh->faces.count; ++fi) {
					ufbx_face& face = mesh->faces[fi];

					tiny_vector<uint32_t> faceIndices((face.num_indices - 2) * 3/*max possible*/);
					uint32_t triCount = ufbx_triangulate_face(faceIndices.data(), faceIndices.size(), mesh, face);

					// Loop through the vertex indices of the face
					for (size_t i = 0; i < triCount * 3; ++i)
					{
						const uint32_t cornerIndex = faceIndices[i];
						const uint32_t vertexIndex = mesh->vertex_indices[cornerIndex];

						const ufbx_vec3& v = mesh->vertices[vertexIndex];
						glm::vec3 vPos = { v.x,v.y,v.z };
						glm::vec3 vNormal = { 0,0,0 };
						glm::vec4 vColor = { 1.f, 1.f, 1.f, 1.f };

						glm::vec2 vTexCoord;
						if (mesh->vertex_uv.exists) {
							const uint32_t uv_index = mesh->vertex_uv.indices[cornerIndex];
							ufbx_vec2 uv = mesh->vertex_uv.values[uv_index];
							vTexCoord = { uv.x, uv.y };
						}

						if (mesh->vertex_normal.exists) {
							const uint32_t nml_index = mesh->vertex_normal.indices[cornerIndex];
							const ufbx_vec3 nml = mesh->vertex_normal.values[nml_index];
							vNormal = { nml.x, nml.y, nml.z };
						}

						tiny_vector<glm::vec2> vUVs = { vTexCoord };

						//// create VT type vertex and load it

						VT vertex;
						if (deduplicateVertices) 
						{
							vertex.loadFromAttributes(vPos, lastIndex, vColor, vNormal, vUVs);

							if (uniqueVertices.count(vertex) == 0) {
								uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
								vertices.push_back(proc, vertex);
							}

							indices.push_back(proc, uniqueVertices[vertex]);
						}
						else {

							vertices.push_back(proc, vertex);
							indices.push_back(proc, lastIndex);
						}
						lastIndex++;
					}
				}
			}
		}

		return VAL_SUCCESS;
	}



	// load from scene, OBJ
	template <typename VT/*vertex type*/, uint16_t NTextures>
	VAL_RETURN_CODE Mesh<VT, NTextures>::importFromScene(ValProc& proc, ObjScene& scene, const uint32_t meshIndex, bool deduplicateVertices)
	{
		const bool triangulate = true;

		vertices.destroy(proc);
		indices.destroy(proc);

		indices.setUsage(proc, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		vertices.setUsage(proc, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

		tinyobj::attrib_t& attrib = scene._attrib;
		std::vector<tinyobj::shape_t>& shapes = scene._shapes;
		std::vector<tinyobj::material_t>& materials = scene._materials;
		std::string warn, err;

		size_t totalIndexCount = 0u;

		// first calculate the total index count
		for (const auto& shape : shapes)
		{
			totalIndexCount += shape.mesh.indices.size();
		}

		if (totalIndexCount == 0) {
			return VAL_SUCCESS; // there no mesh to load
		}

		uint32_t lastIndex = 0u;
		std::unordered_map<VT, uint32_t> uniqueVertices{};
		if (deduplicateVertices == false)
		{
			vertices.resize(proc, uint32_t(totalIndexCount));
			indices.resize(proc, uint32_t(totalIndexCount));
		}


		for (size_t i = 0; i < shapes.size(); ++i)
		{
			const auto& shape = shapes[i];

			for (size_t j = 0; j < shape.mesh.indices.size(); ++j)
			{
				const tinyobj::index_t& index = shape.mesh.indices[j];
				if (3 * index.vertex_index + 2 >= attrib.vertices.size())
				{
					break;
				}

				const glm::vec3 pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				const glm::vec2 texCoord = {
				  attrib.texcoords[2 * index.texcoord_index + 0],
				  1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				const glm::vec4 color = { 1.f,1.f,1.f,1.f };

				if (deduplicateVertices) {
					VT vertex;
					vertex.loadFromAttributes(pos, lastIndex, color, { 0.f,0.f,0.f }, { texCoord });

					if (uniqueVertices.count(vertex) == 0) {
						uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
						vertices.push_back(proc, vertex);
					}

					indices.push_back(proc, uniqueVertices[vertex]);
				}
				else {
					VT* vertex = &vertices[lastIndex];
					vertex->loadFromAttributes(pos, lastIndex, color, { 0.f,0.f,0.f }, { texCoord });
					indices[lastIndex] = lastIndex;
				}

				lastIndex++;

				// add index associated with unique vertex to index list
			}
		}

		return VAL_SUCCESS;
	}
}

#endif // !VAL_MESH_HPP