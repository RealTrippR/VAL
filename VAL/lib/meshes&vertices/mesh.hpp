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

#include <stdlib.h>

#ifndef VAL_TINY_OBJ_H
#define VAL_TINY_OBJ_H
#include <ExternalLibraries/tiny_obj_loader.h>
#endif // !VAL_TINY_OBJ_H

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
			if (indices.getVkBuffer() != NULL || vertices.getVkBuffer())
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

		VAL_RETURN_CODE loadFromFile(ValProc& proc, const std::filesystem::path& filepath, bool deduplicate = true)
		{
			indices.setUsage(proc, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
			vertices.setUsage(proc, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

			std::string ext = filepath.extension().string();
			if (ext == ".fbx") {
				return loadFromFBX(proc, filepath, deduplicate);
			}
			else if (ext == ".obj") {
				return loadFromOBJ(proc, filepath, deduplicate);
			}
			else {
				return VAL_FAILURE;
			}
		}

		void setTexture(ValProc& proc, uint8_t location, const Texture2D& texture) const
		{

		}

		const Texture2D& getTexture(uint8_t location) const
		{

		}

	private:
		VAL_RETURN_CODE loadFromOBJ(ValProc& proc, const std::filesystem::path& filepath, bool deduplicateVertices)
		{
			const bool triangulate = true;

			vertices.destroy(proc);
			indices.destroy(proc);

			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			std::string warn, err;

			if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.string().c_str(), NULL, triangulate))
			{
				dbg::printWarning("Mesh::loadFromOBJ: Failed to load .OBJ mesh for Mesh @ %p. tinyobj err: %s",this, err);
				return VAL_FAILURE;
			}

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
						vertex.loadFromAttributes(pos, lastIndex, color, { 0.f,0.f,0.f }, {texCoord});
						
						if (uniqueVertices.count(vertex) == 0) {
							uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
							vertices.push_back(proc,vertex);
						}

						indices.push_back(proc,uniqueVertices[vertex]);
					}
					else {
						VT* vertex = &vertices[lastIndex];
						vertex->loadFromAttributes(pos, lastIndex, color, { 0.f,0.f,0.f }, {texCoord});
						indices[lastIndex] = lastIndex;
					}

					lastIndex++;

					// add index associated with unique vertex to index list
				}
			}

			return VAL_SUCCESS;
		}

		VAL_RETURN_CODE loadFromFBX(ValProc& proc, const std::filesystem::path& filepath, bool deduplicateVertices)
		{
			const bool triangulate = true;

			vertices.destroy(proc);
			indices.destroy(proc);

			ufbx_load_opts opts = { 0 }; // Optional, pass NULL for defaults
			ufbx_error error; // Optional, pass NULL if you don't care about errors
			ufbx_scene* scene = ufbx_load_file(filepath.string().c_str(), &opts, &error);
			if (!scene) {
				dbg::printWarning("Mesh::loadFromFBX: Failed to load FBX file %s for Mesh @ %p. ufbx error: %s", filepath.u8string().c_str(), this, error.description.data);
				return VAL_FAILURE;
			}

			deduplicateVertices = false;

			size_t totalIndexCount = 0u;

			for (size_t ni = 0; ni < scene->nodes.count; ni++)
			{
				ufbx_node* node = scene->nodes.data[ni];
				if (node->is_root) continue;
				if (node->mesh)
				{
					totalIndexCount += node->mesh->vertices.count;
				}
			}

			if (totalIndexCount == 0) {
				return VAL_SUCCESS; // there no mesh to load
			}

			size_t lastIndex = 0u;
			if (deduplicateVertices) {

			}
			else {
				

				/*vertices.resize(proc, totalIndexCount);
				indices.resize(proc, totalIndexCount);*/
			}


			std::unordered_map<std::filesystem::path,Texture2D> loadedTextures_fromFile;

			std::unordered_map<uint32_t/*textureID*/,Texture2D> loadedTextures_fromEmbedded;

			for (size_t ni = 0; ni < scene->nodes.count; ni++)
			{
				ufbx_node* node = scene->nodes.data[ni];
				if (node->is_root) continue;

				if (node->mesh) 
				{
					ufbx_mesh* mesh = node->mesh;

					for (size_t fi = 0; fi < mesh->faces.count; ++fi)
					{
						ufbx_face& face = mesh->faces[fi];

						tiny_vector<uint32_t> faceIndices((face.num_indices-2) * 3/*max possible*/);
						uint32_t triCount = ufbx_triangulate_face(faceIndices.data(), faceIndices.size(), mesh, face);

						// Loop through the vertex indices of the face
						for (size_t i = 0; i < triCount*3; ++i)
						{

							const uint32_t cornerIndex = faceIndices[i];
							const uint32_t vertexIndex = mesh->vertex_indices[cornerIndex];

							const ufbx_vec3& v = mesh->vertices[vertexIndex];
							glm::vec3 vPos = { v.x,v.y,v.z };

							tiny_vector<glm::vec2> vUVs;
							glm::vec4 vColor = { 1.f, 1.f, 1.f, 1.f };
							glm::vec3 vNormal = { 0,0,0 };

							//// create VT type vertex and load it
							VT vertex;
							vertex.loadFromAttributes(vPos, lastIndex, vColor, vNormal, vUVs);
							
							vertices.push_back(proc, vertex);
							indices.push_back(proc, lastIndex);

							lastIndex++;
						}
					}

					continue;

					// load materials (if applicable)
					for (size_t mi = 0; mi < mesh->materials.count; ++mi)
					{

						ufbx_material* material = mesh->materials[mi];

						// get textures of material
						for (size_t ti = 0; ti < mesh->materials.count; ++ti)
						{
							ufbx_material_texture* matTexture = &material->textures[ti];
							ufbx_texture* texture = matTexture->texture;

							if (texture->type == UFBX_TEXTURE_FILE)
							{
								// it must be retrieved from disk
								if (texture->has_file) 
								{
									// load texture from disk (if it hasn't already)
									//fs::path filepath = fs::path(cstrtexture->absolute_filename.data);

									// first check if the texture is loaded.
									if (loadedTextures_fromFile.count(filepath) == 0) {
										Texture2D& texture2D = loadedTextures_fromFile[filepath];
										texture2D.setValProc(&proc);
										texture2D.createFromDisk(texture->absolute_filename.data,
											VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
											bufferSpace::GPU_ONLY, 1u,
											USE_SOURCE_DIMENSION, USE_SOURCE_DIMENSION);
										//loadedTextures_fromFile[filepath];
									}
								}
								// it's embedded
								if (texture->content.size != 0)
								{
									// load texture from embedded data
								}
							}

							//texture->has_file;
							//// relative to currently loaded file
							//texture->filename;

							//const ufbx_texture_type	textureType = texture->type;
						}
					}
					//ufbx_material* material = material->textures.count
					//material.textures;

					// load UVs (if applicable)







					/*
					ufbx_uv_set_list& uv_sets = mesh->uv_sets;

					for (size_t vi = 0; vi < mesh->vertices.count; ++vi)
					{
						const ufbx_vec3 ufbx_vertex = mesh->vertices[vi];
						const glm::vec3 vPos = { ufbx_vertex.x, ufbx_vertex.y, ufbx_vertex.z };
						tiny_vector<glm::vec2> vUVs;
						glm::vec4 vColor = { 0,0,0,0 };
						glm::vec3 vNormal = { 0,0,0 };


						// create VT type vertex and load it
						VT& vertex = vertices[lastIndex];
						vertex.loadFromAttributes(vPos, lastIndex, vColor, vNormal, vUVs);
						indices[lastIndex] = lastIndex;

						lastIndex++;
					}
					*/




					/*if (mesh->vertex_color.exists)
					{
						ufbx_vec4 color = mesh->vertex_color.values[vi];
						vColor = { color.x,color.y,color.z,color.w };
					}*/

					/*	if (uv_sets.count > 0)
						{
							for (size_t uvi = 0u; uvi < uv_sets.count; ++uvi)
							{
								ufbx_uv_set& uv_set = mesh->uv_sets[uvi];
								ufbx_vec2& ufbx_uv = uv_set.vertex_uv[vi];
								vUVs.push_back({ ufbx_uv.x, ufbx_uv.y });
							}
						}*/


						/*	if (mesh->normals.count > 0)
							{
								ufbx_vec3 ufbx_normal = mesh->normals.data[vi];
								vNormal = { ufbx_normal.x, ufbx_normal.y, ufbx_normal.z };
							}*/


					// get materials
					/*
					ufbx_mesh* mesh = node->mesh;

					// Get the material for the whole mesh (or per-face if using subsets)
					if (mesh->materials.count > 0)
					{
						ufbx_material* material = mesh->material_slots[0].material;
						printf("Material name: %s\n", material->name.data);
					}
					*/
				}
			}

			ufbx_free_scene(scene);


			return VAL_SUCCESS;
		}
	};
}

#endif // !VAL_MESH_HPP