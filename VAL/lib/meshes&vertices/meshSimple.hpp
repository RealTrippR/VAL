#ifndef VAL_SIMPLE_MESH_HPP
#define VAL_SIMPLE_MESH_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// to allow hasing for the vertex3D struct
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/meshes&vertices/vertex3DSimple.hpp>

namespace val {
	namespace fs = std::filesystem;

	class meshSimple {
	public:
		meshSimple(VAL_PROC& proc) : _proc(proc) {};
	public:
		void loadFromDiskObj(VAL_PROC& proc, fs::path objPath, bool deduplicateVertices);

		void cleanup(VAL_PROC& proc);

		std::vector<vertex3Dsimple> _vertices;
		std::vector<uint32_t> _indices;
		tinyobj::attrib_t _meshAttribs;

		VkBuffer _vertexBuffer = NULL;
		VkDeviceMemory _vertexBufferMem = NULL;

		VkBuffer _indexBuffer = NULL;
		VkDeviceMemory _indexBufferMem = NULL;

		val::VAL_PROC& _proc;
	};
}

#endif // !VAL_SIMPLE_MESH_HPP