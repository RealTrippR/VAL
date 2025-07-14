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
#include <VAL/lib/meshes&vertices/plyScene.hpp>

#include <VAL/lib/ext/streql.h>
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
		uint32_t layoutIdx=0u;
	};

	template <typename VT/*vertex type*/, uint16_t NTextures>
	class Mesh
	{
	public:
		using VertexType = VT;
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

		void setVertices(ValProc& proc, const tiny_vector<VT>& vertices, const VkBufferUsageFlags additionalBufferUsages = 0x0);

		void setIndices(ValProc& proc, const tiny_vector<uint32_t>& indices, const VkBufferUsageFlags additionalBufferUsages = 0x0);

		VAL_RETURN_CODE importFromScene(ValProc& proc, ufbx_scene* scene, const uint32_t meshIndex,
				const VkBufferUsageFlags additionalVertexBufferUsages = 0x0, const VkBufferUsageFlags additionalIndexBufferUsages = 0x0, bool deduplicateVertices = true);

		VAL_RETURN_CODE importFromScene(ValProc& proc, ObjScene& scene, const uint32_t meshIndex,
				const VkBufferUsageFlags additionalVertexBufferUsages = 0x0, const VkBufferUsageFlags additionalIndexBufferUsages = 0x0, bool deduplicateVertices = true);

		VAL_RETURN_CODE importFromScene(ValProc& proc, const val::PlyScene& scene,
			const VkBufferUsageFlags additionalVertexBufferUsages = 0x0, const VkBufferUsageFlags additionalIndexBufferUsages = 0x0);

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
	VAL_RETURN_CODE Mesh<VT, NTextures>::importFromScene(ValProc& proc, ufbx_scene* scene, const uint32_t meshIndex, const VkBufferUsageFlags additionalVertexBufferUsages, const VkBufferUsageFlags additionalIndexBufferUsages, bool deduplicateVertices)
	{
		vertices.destroy(proc);
		indices.destroy(proc);

		vertices.setUsages(proc, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | additionalVertexBufferUsages);
		indices.setUsages(proc, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | additionalIndexBufferUsages);

		size_t totalIndexCount = 0u;
		for (size_t ni = 0; ni < scene->nodes.count; ni++) {
			ufbx_node* node = scene->nodes.data[ni];
			if (node->is_root) continue;
			if (node->mesh) {
				totalIndexCount += node->mesh->vertices.count;
			}
		}
		if (totalIndexCount > UINT32_MAX) {
			dbg::printWarning("mesh::importFromScene: Failed to load mesh, index count exceeds UINT32_MAX");
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
	VAL_RETURN_CODE Mesh<VT, NTextures>::importFromScene(ValProc& proc, ObjScene& scene, const uint32_t meshIndex, const VkBufferUsageFlags additionalVertexBufferUsages, const VkBufferUsageFlags additionalIndexBufferUsages, bool deduplicateVertices)
	{
		const bool triangulate = true;

		vertices.destroy(proc);
		indices.destroy(proc);


		vertices.setUsages(proc, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | additionalVertexBufferUsages);
		indices.setUsages(proc, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | additionalIndexBufferUsages);

		tinyobj::attrib_t& attrib = scene._attrib;
		std::vector<tinyobj::shape_t>& shapes = scene._shapes;
		std::vector<tinyobj::material_t>& materials = scene._materials;
		std::string warn, err;

		if (meshIndex >= shapes.size() || shapes.size() == 0)
		{
			dbg::printError("Mesh::importFromScene: Failed, invalid meshIndex %lu exceeds mesh count of %lu.", meshIndex, shapes.size());
			return VAL_FAILURE;
		}


		size_t totalIndexCount = 0u;

		totalIndexCount += shapes[meshIndex].mesh.indices.size();
		

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


		auto& shape = shapes[meshIndex];

		// create normals if they don't exist
		if (attrib.normals.empty())
		{
			attrib.normals.resize(attrib.vertices.size(), 0.0f);

			for (size_t i = 0; i < shape.mesh.indices.size(); i += 3)
			{
				tinyobj::index_t& index = shape.mesh.indices[i];

				glm::vec3 pos0 = {
					attrib.vertices[3 * i + 0],
					attrib.vertices[3 * i + 1],
					attrib.vertices[3 * i + 2]
				};
				glm::vec3 pos1 = {
						attrib.vertices[3 * (i + 1) + 0],
						attrib.vertices[3 * (i + 1) + 1],
						attrib.vertices[3 * (i + 1) + 2]
				};
				glm::vec3 pos2 = {
					attrib.vertices[3 * (i + 2) + 0],
					attrib.vertices[3 * (i + 2) + 1],
					attrib.vertices[3 * (i + 2) + 2]
				};

				glm::vec3 n = glm::normalize(glm::cross((pos1 - pos0), (pos2 - pos0)));

				attrib.normals[i + 0] = n.x;
				attrib.normals[i + 1] = n.y;
				attrib.normals[i + 2] = n.z;

				index.normal_index = i / 3;
			}
		}



		for (size_t j = 0; j < shape.mesh.indices.size(); ++j)
		{
			const tinyobj::index_t& index = shape.mesh.indices[j];
			if (3 * index.vertex_index + 2 >= attrib.vertices.size())
			{
				break;
			}
			const glm::vec3& normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};

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
				vertex.loadFromAttributes(pos, lastIndex, color, normal, { texCoord });

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(proc, vertex);
				}

				indices.push_back(proc, uniqueVertices[vertex]);
			}
			else {
				VT* vertex = &vertices[lastIndex];
				vertex->loadFromAttributes(pos, lastIndex, color, normal, { texCoord });
				indices[lastIndex] = lastIndex;
			}

			lastIndex++;

			// add index associated with unique vertex to index list
		}

		return VAL_SUCCESS;
	}






	// load from scene, PLY
	template <typename VT/*vertex type*/, uint16_t NTextures>
	VAL_RETURN_CODE Mesh<VT, NTextures>::importFromScene(ValProc& proc, const val::PlyScene& scenePLYVAL,
		const VkBufferUsageFlags additionalVertexBufferUsages, const VkBufferUsageFlags additionalIndexBufferUsages)
	{
		vertices.destroy(proc);
		indices.destroy(proc);

		vertices.setUsages(proc, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | additionalVertexBufferUsages);
		indices.setUsages(proc, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | additionalIndexBufferUsages);


		// read more
		// https://paulbourke.net/dataformats/ply/ 
		// see UNC paper about suggested extensions (color, normal, etc)
		// https://gamma.cs.unc.edu/POWERPLANT/papers/ply.pdf

		uint32_t lastIndex = 0u;
		
		using namespace cply;
		const cply::PlyScene* scene = scenePLYVAL.getPlyScene();

		bool hasNormals = false;

		for (U64 eId = 0; eId < scene->elementCount; ++eId)
		{
			PlyElement* ele = scene->elements + eId;
			for (U64 lno = 0; lno < ele->dataLineCount; ++lno)
			{
				if (streql(ele->name, "vertex")) 
				{
					glm::vec3 pos = { 0,0,0 };
					glm::vec3 normal = { 0,0,0 };
					glm::vec4 color = { 1.f,1.f,1.f,1.f };

					for (U64 pId = 0; pId < ele->propertyCount; ++pId)
					{
						PlyProperty* prop = ele->properties + pId;
						// pos x
						if (streql(prop->name, "x"))
						{
							double val = scenePLYVAL.getDataFromPropertyOfElement(ele, prop, lno, NULL);
							pos.x = (float)val;
						}
						// pos y
						else if (streql(prop->name, "y"))
						{
							double val = scenePLYVAL.getDataFromPropertyOfElement(ele, prop, lno, NULL);
							pos.y = (float)val;
						}
						// pos z
						else if (streql(prop->name, "z"))
						{
							double val = scenePLYVAL.getDataFromPropertyOfElement(ele, prop, lno, NULL);
							pos.z = (float)val;
						}
						// normal x
						else if (streql(prop->name, "nx"))
						{
							double val = scenePLYVAL.getDataFromPropertyOfElement(ele, prop, lno, NULL);
							normal.x = (float)val;
							hasNormals = true;
						}
						// normal y
						else if (streql(prop->name, "ny"))
						{
							double val = scenePLYVAL.getDataFromPropertyOfElement(ele, prop, lno, NULL);
							normal.y = val;
							hasNormals = true;
						}
						// normal z
						else if (streql(prop->name, "nz"))
						{
							double val = scenePLYVAL.getDataFromPropertyOfElement(ele, prop, lno, NULL);
							normal.z = (float)val;
							hasNormals = true;
						}
						// red color
						else if (streql(prop->name, "red"))
						{
							double val = scenePLYVAL.getDataFromPropertyOfElement(ele, prop, lno, NULL);
							color.r = (float)val;
						}
						// green color
						else if (streql(prop->name, "green"))
						{
							double val = scenePLYVAL.getDataFromPropertyOfElement(ele, prop, lno, NULL);
							color.g = (float)val;
						}
						// blue color
						else if (streql(prop->name, "blue"))
						{
							double val = scenePLYVAL.getDataFromPropertyOfElement(ele, prop, lno, NULL);
							color.b = (float)val;
						}
						// alpha color
						else if (streql(prop->name, "alpha"))
						{
							double val = scenePLYVAL.getDataFromPropertyOfElement(ele, prop, lno, NULL);
							color.a = (float)val;
						}
					}

					const glm::vec2 texCoord = {
						0,
						0
					};


					VT vertex;
					vertex.loadFromAttributes(pos, lastIndex, color, normal, { texCoord });
					vertices.push_back(proc,vertex);
				}
				else if (streql(ele->name, "face"))
				{
					for (U64 pId = 0; pId < ele->propertyCount; ++pId)
					{
						PlyProperty* prop = ele->properties + pId;
						if (prop->dataType == PLY_DATA_TYPE_LIST) {
							U64 indexArrCount = 0u;
							scenePLYVAL.getDataFromPropertyOfElementAsList(NULL, NULL, &indexArrCount, ele, prop, lno, NULL);
							
							if (!indexArrCount) // nothing to alloc
								continue;
							
							double* data = (double*)calloc(indexArrCount, sizeof(double));
							if (!data)
								return VAL_FAILURE;

							scenePLYVAL.getDataFromPropertyOfElementAsList(data, indexArrCount * sizeof(double), NULL, ele, prop, lno, NULL);

							if (indexArrCount < 3)
								goto skip_loop; // indices don't make a valid face


							
							// do a fan triangulation: for a face with N indices, create (N - 2) triangles
							for (U64 i = 1; i < indexArrCount - 1; ++i)
							{
								indices.push_back(proc, data[0]);
								indices.push_back(proc, data[i]);
								indices.push_back(proc, data[i + 1]);

								// perform normal calculation (if normals don't already exist)
								if (hasNormals == false) {

									glm::vec3 p0 = vertices[data[0]].getPositionAsVec3();
									glm::vec3 p1 = vertices[data[i]].getPositionAsVec3();
									glm::vec3 p2 = vertices[data[i + 1]].getPositionAsVec3();
									
									glm::vec3 n = glm::normalize(glm::cross((p1 - p0), (p2 - p0)));

									vertices[data[0]].setNormalFromVec3(n);
									vertices[data[i]].setNormalFromVec3(n);
									vertices[data[i + 1]].setNormalFromVec3(n);
								}
							}

						skip_loop:
							free(data);
						}
					}
				}
			}
		}

		return VAL_SUCCESS;
	}


	template <typename VT/*vertex type*/, uint16_t NTextures>
	void Mesh<VT, NTextures>::setVertices(ValProc& proc, const tiny_vector<VT>& _vertices, const VkBufferUsageFlags usages)
	{
		vertices.destroy(proc);
		vertices.setUsages(proc, usages | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		vertices.resize(proc, _vertices.size());
		
		memcpy(vertices.data(), _vertices.data(), _vertices.size() * sizeof(VT));
	}

	template <typename VT/*vertex type*/, uint16_t NTextures>
	void Mesh<VT, NTextures>::setIndices(ValProc& proc, const tiny_vector<uint32_t>& _indices, const VkBufferUsageFlags usages)
	{
		indices.destroy(proc);
		indices.setUsages(proc, usages | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		indices.resize(proc,_indices.size());

		memcpy(indices.data(), _indices.data(), _indices.size() * sizeof(uint32_t));
	}

}

#endif // !VAL_MESH_HPP