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

#include <VAL/lib/meshes&vertices/meshTextured.hpp>
#include <VAL/lib/meshes&vertices/loadModels.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {

	void meshTextured::loadFromDiskObj(VAL_PROC& proc, fs::path objPath, bool deduplicateVertices) {

		loadModelFromDiskAsUnifiedMesh(objPath, _vertices, _indices, &_meshAttribs, deduplicateVertices);

		proc.createVertexBuffer(_vertices.data(), _vertices.size(), sizeof(val::vertex3D), &_vertexBuffer, &_vertexBufferMem);

		proc.createIndexBuffer(_indices.data(), _indices.size(), &_indexBuffer, &_indexBufferMem);
	}

	void meshTextured::cleanup(VAL_PROC& proc) {
		VkDevice& device = proc._device;

#ifndef NDEBUG
		if (!(bool(_indexBuffer) xor bool(_vertexBuffer))) {
			printf("VAL: Improper deallocation of Vulkan allocated memory: Index buffer or Vertex Buffer is invalid.\n\
			Both must be NULL, or both must point to a valid address.");
		}
#endif // !NDEBUG

		if (_indexBuffer) {
			vkDestroyBuffer(device, _indexBuffer, NULL);
			vkFreeMemory(device, _indexBufferMem, NULL);

			vkDestroyBuffer(device, _indexBuffer, NULL);
			vkFreeMemory(device, _indexBufferMem, NULL);
		}
	}

	void meshTextured::setTexture(VAL_PROC& proc, val::image* texture) {
		_texture = texture;
		if (_textureImageView) {
			vkDestroyImageView(proc._device, _textureImageView, NULL);
		}
		_textureImageView.create(*texture, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}