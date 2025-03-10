#include <VAL/lib/meshes&vertices/meshTextured.hpp>

namespace val {

	void meshTextured::loadFromDiskObj(VAL_PROC& proc, fs::path objPath, bool deduplicateVertices) {

		loadModelFromDiskAsUnifiedMesh(objPath, _vertices, _indices, _meshAttribs);

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
		_textureImageView.recreate(*texture, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}