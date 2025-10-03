#include <VAL/lib/meshes&vertices/fbxScene.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/texture2d.inl>
#include <VAL/lib/ext/streql.h>

#include <stdlib.h>
#include <stdio.h>

namespace val
{
	void printTextureUFBX(ufbx_texture* texture) {
		printf("\n====== TEXTURE DATA ======\n");
		printf("name: %s\n", texture->name.data);
		printf("absolute filename: %s\n", texture->absolute_filename.data);
		printf("filename: %s\n", texture->filename.data);
		printf("file index: %lu\n", texture->file_index);
		printf("type: ");
		if (texture->type == UFBX_TEXTURE_FILE) {
			printf("file");
		}
		else if (texture->type == UFBX_TEXTURE_LAYERED)
		{
			printf("layered");
		}
		else if (texture->type == UFBX_TEXTURE_PROCEDURAL)
		{
			printf("procedural");
		}
		else if (texture->type == UFBX_TEXTURE_SHADER)
		{
			printf("shader");
		}
		printf("\n");

		printf("props (total of %llu): \n", texture->props.props.count);
		for (size_t i = 0; i < texture->props.props.count; ++i)
		{
			ufbx_prop* prop = &texture->props.props[i];
			
			printf("\t\tprop no. %llu: \n", i);
			printf("\t\tprop name: %s\n",prop->name.data);

			printf("\t\tprop type: ");
			if (prop->type == UFBX_PROP_BOOLEAN) {
				printf("boolean");
			}
			else if (prop->type == UFBX_PROP_COLOR_WITH_ALPHA) {
				printf("color with alpha");
			}
			else if (prop->type == UFBX_PROP_COLOR) {
				printf("color");
			}
			else if (prop->type == UFBX_PROP_INTEGER) {
				printf("integer");
			}
			else if (prop->type == UFBX_PROP_NUMBER) {
				printf("number");
			}
			else if (prop->type == UFBX_PROP_COMPOUND) {
				printf("compound");
			}
			else if (prop->type == UFBX_PROP_DATE_TIME) {
				printf("date-time");
			}
			else if (prop->type == UFBX_PROP_DISTANCE) {
				printf("distance");
			}
				
			printf("\n");
			printf("\t----------------\n");
		}
	}

	void createValTexture2D_FromUFBX_Texture(Queue &q, ufbx_texture* textureFbx, val::Texture2D* texture)
	{
		if (textureFbx->content.size >= UINT32_MAX)
		{
			dbg::printWarning("createValTexture2D_FromUFBX_Texture: Failed to create texture, it's content size exceeds UINT32_MAX (~4GB)");
			return;
		}

		if (textureFbx->content.size != 0)
		{	// load texture from embedded data
			texture->createFromMemory(q,textureFbx->content.data, (uint32_t)textureFbx->content.size,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, texture->getImageLayout(),
				BUFFER_SPACE::GPU_ONLY, 1u);
			if (texture->getVkImage() != VK_NULL_HANDLE) {
				return;
			}
		}
		if (textureFbx->has_file) { 
		//// it's stored on the disk / embed fail fallback (fbx textures can have both embedded data and an associated file)
			texture->createFromDisk(q,textureFbx->absolute_filename.data,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, texture->getImageLayout(),
				BUFFER_SPACE::GPU_ONLY, 1u,
				USE_SOURCE_DIMENSION, USE_SOURCE_DIMENSION);
		}
	}

	void FbxScene::destroy() {
		if (_scene)
		{
			ufbx_free_scene(_scene);
			_scene = NULL;
		}
	}

	VAL_RETURN_CODE FbxScene::loadFromDisk(const fs::path& path)
	{
		FILE* fptr=NULL;
		errno_t fErr = 0x0;
		#ifdef _WIN32
		fErr= _wfopen_s(&fptr, path.c_str(), L"rb");
		#else
		fErr=fopen_s(&fptr, path.string().c_str(), "rb");
		#endif

		if (!fptr) {
			dbg::printError("FbxScene::loadFromDisk: Failed to open fbx scene from disk.");
			return VAL_FAILURE;
		}

		fseek(fptr, 0L, SEEK_END);
		size_t fileSize = ftell(fptr);

		rewind(fptr);

		tiny_vector<char> fileData((uint32_t)fileSize);
		fread_s(fileData.data(), fileData.size(), fileData.size(), 1u, fptr);

		loadFromMemory(fileData.data(), fileData.size());

		fileData.~tiny_vector();

		fclose(fptr);

		return VAL_SUCCESS;
	}

	VAL_RETURN_CODE FbxScene::loadFromMemory(const void* memory, const size_t memByteLen)
	{
		ufbx_load_opts opts = { 0 }; // Optional, pass NULL for defaults
		ufbx_error error; // Optional, pass NULL if you don't care about errors
		_scene = ufbx_load_memory(memory, memByteLen, &opts, &error);

		if (_scene == NULL) {
			dbg::printError("FbxScene::loadFromMemory: Failed to open fbx scene. Ufbx error: %s", error.description.data);
			return VAL_FAILURE;
		}
		return VAL_SUCCESS;
	}

	VAL_RETURN_CODE FbxScene::importTexture2D(Queue& q, Texture2D* texture, ValProc& proc, const uint32_t meshIndex, const uint32_t materialIndex, const FBX_MATERIAL_PROPERTY texProperty)
	{
		const std::vector<const char*> texPropertyAsStringList = FBX_MATERIAL_PROPERTY_toStringList(texProperty);

		return importTexture2D(q, texture, proc, meshIndex, materialIndex, texPropertyAsStringList);
	}
	
	VAL_RETURN_CODE FbxScene::importTexture2D(Queue& q, Texture2D* texture, ValProc& proc, const uint32_t meshIndex, const uint32_t materialIndex, const char* texProperty)
	{
		return importTexture2D(q, texture, proc, meshIndex, materialIndex, std::vector<const char*> { texProperty });
	}

	VAL_RETURN_CODE FbxScene::importTexture2D(Queue& q, Texture2D* texture, ValProc& proc, const uint32_t meshIndex, const uint32_t materialIndex, std::vector<const char*> texPropertyMatchList)
	{
		ufbx_scene* scene = _scene;

		throw std::runtime_error("IMPLEMENT THIS!");
		// Compute normal when no normal were provided.
		/*if (attrib.normals.empty())
		{
			for (size_t i = 0; i < m_indices.size(); i += 3)
			{
				VertexObj& v0 = m_vertices[m_indices[i + 0]];
				VertexObj& v1 = m_vertices[m_indices[i + 1]];
				VertexObj& v2 = m_vertices[m_indices[i + 2]];

				glm::vec3 n = glm::normalize(glm::cross((v1.pos - v0.pos), (v2.pos - v0.pos)));
				v0.nrm = n;
				v1.nrm = n;
				v2.nrm = n;
			}
		}*/
		for (size_t ni = 0; ni < scene->nodes.count; ni++)
		{
			ufbx_node* node = scene->nodes.data[ni];
			if (node->is_root) continue;
			if (node->mesh) {
				ufbx_mesh* mesh = node->mesh;

				if (materialIndex >= mesh->materials.count) {
					return VAL_FAILURE;
				}

				ufbx_material* materialFbx = mesh->materials[materialIndex];

				for (size_t ti = 0; ti < materialFbx->textures.count; ++ti)
				{
					ufbx_material_texture* matTextureFbx = &materialFbx->textures[ti];
					ufbx_texture* textureFbx = matTextureFbx->texture;

					

					for (size_t mti = 0; mti < texPropertyMatchList.size(); mti++)
					{
						const char* curTexProp = texPropertyMatchList[mti];
						if (streql(curTexProp, matTextureFbx->material_prop.data))
						{
							createValTexture2D_FromUFBX_Texture(q, textureFbx, texture);
						}
					}
				}
				/*
				ufbx_prop_list* props = &materialFbx->props.props;
				for (size_t pi = 0; pi < props->count; ++pi)
				{
					ufbx_prop* prop = &props->data[pi];

					
					for (size_t mti = 0; mti < texPropertyMatchList.size(); mti++)
					{
						const char* curTexProp = texPropertyMatchList[mti];
						if (streql(curTexProp, prop->name.data))
						{
							ufbx_texture* textureFbx = matTextureFbx->texture;
							createValTexture2D_FromUFBX_Texture(textureFbx, texture);
						}
					}

					//texPropertyAsStringList
				}
				*/
			//	ufbx_material_texture* matTextureFbx = &materialFbx->textures[textureIndex];
				//ufbx_texture* textureFbx = matTextureFbx->texture;

				/*if (textureFbx->type == UFBX_TEXTURE_FILE)
				{
					createValTexture2D_FromUFBX_Texture(textureFbx, texture);
				}*/
			}
		}

		return VAL_SUCCESS;
	}

	VAL_RETURN_CODE FbxScene::importTexture2D(Queue& q, Texture2D* texture, ValProc& proc, const uint32_t meshIndex, const uint32_t materialIndex, const uint32_t textureIndex)
	{
		ufbx_scene* scene = _scene;

		for (size_t ni = 0; ni < scene->nodes.count; ni++)
		{
			ufbx_node* node = scene->nodes.data[ni];
			if (node->is_root) continue;
			if (node->mesh) {
				ufbx_mesh* mesh = node->mesh;

				if (materialIndex >= mesh->materials.count) {
					return VAL_FAILURE;
				}

				ufbx_material* materialFbx = mesh->materials[materialIndex];

				if (textureIndex >= materialFbx->textures.count) {
					return VAL_FAILURE;
				}
				ufbx_material_texture* matTextureFbx = &materialFbx->textures[textureIndex];
				ufbx_texture* textureFbx = matTextureFbx->texture;

				if (textureFbx->type == UFBX_TEXTURE_FILE)
				{
					createValTexture2D_FromUFBX_Texture(q, textureFbx, texture);
				}
			}
		}

		return VAL_SUCCESS;
	}

	ufbx_scene* FbxScene::getUfbxScene() const
	{
		return _scene;
	}

	void FbxScene::dbgPrintTexturesOfMaterialOfMesh(const uint32_t meshIndex, const uint32_t materialIndex) const
	{
		ufbx_scene* scene = _scene;

		uint32_t currentMeshIndex = 0u;
		for (size_t ni = 0; ni < scene->nodes.count; ni++)
		{
			ufbx_node* node = scene->nodes.data[ni];
			if (node->is_root) continue;
			if (node->mesh) 
			{
				if (currentMeshIndex > meshIndex) {
					printf("Invalid mesh index, texture information will not be printed.\n\n");
					return;
				}

				if (currentMeshIndex == meshIndex) 
				{
					ufbx_mesh* mesh = node->mesh;

					if (materialIndex >= mesh->materials.count) {
						printf("Invalid material index, texture information will not be printed.\n\n");
						return;
					}

					ufbx_material* materialFbx = mesh->materials[materialIndex];


					for (size_t ti = 0; ti < materialFbx->textures.count; ++ti)
					{
						ufbx_material_texture* matTextureFbx = &materialFbx->textures[ti];
						ufbx_texture* textureFbx = matTextureFbx->texture;

						printf("=== MATERIAL TEXTURE #%llu ===\n", ti);
						printf("\tmaterial texture prop: %s\n", matTextureFbx->material_prop.data);
						printf("\tmaterial shader prop: %s\n", matTextureFbx->shader_prop.data);
						printTextureUFBX(textureFbx);
					}
				}

				currentMeshIndex++;
			}
		}
	}

	void FbxScene::dbgPrintMaterialsOfMesh(const uint32_t meshIndex) const
	{
		ufbx_scene* scene = _scene;

		uint32_t currentMeshIndex = 0u;
		for (size_t ni = 0; ni < scene->nodes.count; ni++)
		{
			ufbx_node* node = scene->nodes.data[ni];
			if (node->is_root) continue;
			if (node->mesh)
			{
				if (currentMeshIndex > meshIndex) {
					printf("Invalid mesh index, texture information will not be printed.\n\n");
					return;
				}

				if (currentMeshIndex == meshIndex) 
				{
					ufbx_mesh* mesh = node->mesh;

					for (size_t mi = 0; mi < mesh->materials.count; ++mi)
					{
						ufbx_material* materialFbx = mesh->materials[mi];


						printf("=== MATERIAL #%llu ===\n", mi);
						printf("\tname: %s\n", materialFbx->name.data);
						//printf("\tpbr information:", materialFbx->pbr.)
					}
				}

				currentMeshIndex++;
			}
		}
	}

	void FbxScene::dbgPrintMaterialsAndTexturesOfMesh(const uint32_t meshIndex) const
	{
		ufbx_scene* scene = _scene;

		uint32_t currentMeshIndex = 0u;
		for (size_t ni = 0; ni < scene->nodes.count; ni++)
		{
			ufbx_node* node = scene->nodes.data[ni];
			if (node->is_root) continue;
			if (node->mesh)
			{
				if (currentMeshIndex > meshIndex) {
					printf("Invalid mesh index, texture information will not be printed.\n\n");
					return;
				}

				if (currentMeshIndex == meshIndex)
				{
					ufbx_mesh* mesh = node->mesh;

					for (size_t mi = 0; mi < mesh->materials.count; ++mi)
					{
						ufbx_material* materialFbx = mesh->materials[mi];

						printf("=== MATERIAL #%llu ===\n", mi);
						printf("\tname: %s\n", materialFbx->name.data);

						for (size_t ti = 0; ti < materialFbx->textures.count; ++ti)
						{
							ufbx_material_texture* matTextureFbx = &materialFbx->textures[ti];
							ufbx_texture* textureFbx = matTextureFbx->texture;

							printf("=== MATERIAL TEXTURE #%llu ===\n", ti);
							printf("\tmaterial texture prop: %s\n", matTextureFbx->material_prop.data);
							printf("\tmaterial shader prop: %s\n", matTextureFbx->shader_prop.data);
							printTextureUFBX(textureFbx);
						}
					}
				}

				currentMeshIndex++;
			}
		}
	}
}