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

#include <VAL/lib/meshes&vertices/plyScene.hpp>

namespace val
{
	VAL_RETURN_CODE PlyScene::loadFromMemory(const void* memoryBegin, size_t memorySize)
	{
		using namespace cply;
		const PlyResult r = PlyLoadFromMemory((const U8*)memoryBegin, memorySize, &_scene, NULL); if (r == PLY_SUCCESS)
		{
			return VAL_SUCCESS;
		}
		else {
			return VAL_FAILURE;
		}
	}

	VAL_RETURN_CODE PlyScene::loadFromDisk(const fs::path& filepath)
	{
		using namespace cply;
		

#ifdef _WIN32
		const PlyResult r = PlyLoadFromDiskW(filepath.native().c_str(), &_scene, NULL);
#else
		const PlyResult r = PlyLoadFromDisk(filepath.native().c_str(), &_scene);
#endif

		if (r == PLY_SUCCESS) {
			return VAL_SUCCESS;
		}
		else {
#ifndef NDEBUG
			dbg::printWarning("PlyScene::loadFromDisk Failed to load PlyScene @ %p from file: %s", this, filepath.string().c_str());
#endif // !NDEBUG
			return VAL_FAILURE;
		}
	}

	void PlyScene::destroy()
	{
		using namespace cply;
		PlyDestroyScene(&_scene);
	}
}