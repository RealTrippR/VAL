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

#ifndef VAL_OBJ_SCENE_HPP
#define VAL_OBJ_SCENE_HPP
#include <ExternalLibraries/tiny_obj_loader.h>

#include <VAL/lib/system/system_utils.hpp>

namespace val
{
	class ObjScene
	{
	public:
		ObjScene() = default;
		ObjScene(const fs::path& source)
		{
			loadFromDisk(source);
		}
		ObjScene(void* memory, const size_t memorySize)
		{
			loadFromMemory(memory, memorySize);
		}
		~ObjScene()
		{
			destroy();
		}

	public:
		VAL_RETURN_CODE loadFromMemory(void* memoryBegin, size_t memorySize);

		VAL_RETURN_CODE loadFromDisk(const fs::path& filepath);

		void destroy();

	public:
		void dbgListShapes();

	public:
		tinyobj::attrib_t _attrib;
		std::vector<tinyobj::shape_t> _shapes;
		std::vector<tinyobj::material_t> _materials;
	};
}

#endif // !VAL_OBj_SCENE_HPP
