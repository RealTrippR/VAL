#include <VAL/lib/meshes&vertices/loadModels.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void meshSimple::loadFromDiskObj(VAL_PROC& proc, fs::path objPath, bool deduplicateVertices) {

		loadModelFromDiskAsUnifiedMesh(objPath, _vertices, _indices, &_meshAttribs, deduplicateVertices);

		proc.createVertexBuffer(_vertices.data(), _vertices.size(), sizeof(val::vertex3D), &_vertexBuffer, &_vertexBufferMem);

		proc.createIndexBuffer(_indices.data(), _indices.size(), &_indexBuffer, &_indexBufferMem);
	}

	void meshSimple::cleanup(VAL_PROC& proc)
	{
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

}