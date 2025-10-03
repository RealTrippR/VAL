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

#ifndef VAL_FBX_SCENE_HPP
#define VAL_FBX_SCENE_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/ext/ubfx.h>
#include <VAL/lib/system/texture2d.hpp>
#include <VAL/lib/meshes&vertices/fbxMaterialProperty.hpp>

namespace val
{
	class FbxScene
	{
	public:
		FbxScene() = default;
		FbxScene(const fs::path& source)
		{
			loadFromDisk(source);
		}
		FbxScene(void* memory, const size_t memorySize)
		{
			loadFromMemory(memory, memorySize);
		}
		~FbxScene()
		{
			destroy();
		}

		operator ufbx_scene* () {
			return _scene;
		}

	public:
		void destroy();
		
		VAL_RETURN_CODE loadFromDisk(const fs::path& source);

		VAL_RETURN_CODE loadFromMemory(const void* memory, const size_t memByteLen);

		VAL_RETURN_CODE importTexture2D(Queue& q, Texture2D* texture, ValProc& proc, const uint32_t meshIndex, const uint32_t materialIndex, const FBX_MATERIAL_PROPERTY texProperty);

		VAL_RETURN_CODE importTexture2D(Queue& q, Texture2D* texture, ValProc& proc, const uint32_t meshIndex, const uint32_t materialIndex, const char* texProperty);

		VAL_RETURN_CODE importTexture2D(Queue& q, Texture2D* texture, ValProc& proc, const uint32_t meshIndex, const uint32_t materialIndex, std::vector<const char*> texPropertyMatchList);

		VAL_RETURN_CODE importTexture2D(Queue& q, Texture2D* texture, ValProc& proc, const uint32_t meshIndex, const uint32_t materialIndex, const uint32_t textureIndex);

		ufbx_scene* getUfbxScene() const;
	
	public:
		void dbgPrintTexturesOfMaterialOfMesh(const uint32_t meshIndex, const uint32_t materialIndex) const;

		void dbgPrintMaterialsOfMesh(const uint32_t meshIndex) const;
		
		void dbgPrintMaterialsAndTexturesOfMesh(const uint32_t meshIndex) const;

	private:
		ufbx_scene* _scene = NULL;
	};
}
#endif // !VAL_FBX_SCENE_HPP
