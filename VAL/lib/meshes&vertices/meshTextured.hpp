#ifndef VAL_MESH_TEXTURED_HPP
#define VAL_MESH_TEXTURED_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/image.hpp>
#include <VAL/lib/meshes&vertices/vertex3Dtextured.hpp>

namespace val {
	namespace fs = std::filesystem;

	class meshTextured {
	public:
		meshTextured(VAL_PROC& proc) : _proc(proc), _textureImageView(proc) {};
	public:
		void loadFromDiskObj(VAL_PROC& proc, fs::path objPath, bool deduplicateVertices);
		//void loadTexture(VAL_PROC& proc, const fs::path texturePath, const VkFormat imageFormat,
		//	const uint32_t mipLevels = 1u, const VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT);

		void setTexture(VAL_PROC& proc, val::image* texture);
		void cleanup(VAL_PROC& proc);

		std::vector<vertex3D> _vertices;
		std::vector<uint32_t> _indices;
		tinyobj::attrib_t _meshAttribs;

		VkBuffer _vertexBuffer = NULL;
		VkDeviceMemory _vertexBufferMem = NULL;

		VkBuffer _indexBuffer = NULL;
		VkDeviceMemory _indexBufferMem = NULL;

		val::image* _texture = NULL;
		val::imageView _textureImageView;
		val::VAL_PROC& _proc;
		//mainProc.createVertexBuffer(vertices.data(), vertices.size(), sizeof(val::vertex3D), &vertexBuffer, &vertexBufferMem);
	};
}

#endif // !VAL_MESH_TEXTURED_HPP