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

#include <VAL/lib/meshes&vertices/objScene.hpp>

#include <streambuf>
#include <istream>
#include <stdlib.h>

namespace val
{

	//https://stackoverflow.com/questions/13059091/creating-an-input-stream-from-constant-memory
	struct membuf : std::streambuf {
		membuf(const char* base, size_t size) {
			char* p = const_cast<char*>(base);
			this->setg(p, p, p + size);
		}
	};

	class imemstream : public std::istream {
	public:
		imemstream(const char* base, size_t size)
			: std::istream(nullptr), _buffer(base, size) {
			this->rdbuf(&_buffer);
		}
	private:
		membuf _buffer;
	};


	VAL_RETURN_CODE ObjScene::loadFromMemory(void* memoryBegin, size_t memorySize)
	{
		const bool triangulate = true;

		std::string warn;
		std::string err;

		imemstream instream((const char*)memoryBegin, memorySize);

		if (!tinyobj::LoadObj(&_attrib, &_shapes, &_materials, &warn, &err, &instream, NULL, triangulate))
		{
			dbg::printWarning("Mesh::loadFromOBJ: Failed to load .OBJ mesh for Mesh @ %p. tinyobj err: %s", this, err);
			return VAL_FAILURE;
		}

		return VAL_SUCCESS;
	}


	VAL_RETURN_CODE ObjScene::loadFromDisk(const fs::path& filepath)
	{
		FILE* fptr;
#ifdef _WIN32
		_wfopen_s(&fptr, filepath.c_str(), L"rb");
#else
		fopen_s(&fptr, filepath.string().c_str(), "rb");
#endif
		if (!fptr) {
			dbg::printError("ObjScene::loadFromDisk: Failed to open obj scene from disk.");
			return VAL_FAILURE;
		}

		fseek(fptr, 0, SEEK_END);
		size_t fileSize = ftell(fptr);
		rewind(fptr);

		std::vector<char> fileContents(fileSize);

		fread_s(fileContents.data(), fileContents.size(), fileContents.size(), 1, fptr);

		loadFromMemory(fileContents.data(), fileContents.size());

		fclose(fptr);
		return VAL_SUCCESS;
	}
	
	void ObjScene::destroy()
	{
		_attrib.~attrib_t();
		_shapes.~vector();
		_materials.~vector();
	}

	void ObjScene::dbgListShapes()
	{
		for (size_t i = 0; i < _shapes.size(); ++i)
		{
			tinyobj::shape_t& shapes = _shapes[i];
			printf("\n===== SHAPE %llu =====\n", i);
			printf("\tname: %s\n", shapes.name.c_str());
			printf("\tline count: %llu\n", shapes.lines.num_line_vertices);
			printf("\tindex count: %llu\n", shapes.mesh.indices.size());
			printf("\tmaterial count: %llu\n", shapes.mesh.material_ids.size());
			printf("\t----------------\n");
		}
	}
}