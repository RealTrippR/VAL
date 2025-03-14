#ifndef FML_VERTEX_SHADER_HPP
#define FML_VERTEX_SHADER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <FML/lib/system/system_utils.hpp>
#include <FML/lib/graphics/shader.hpp>

#include <vector>
#include <string>
#include <filesystem>

/*
THE DIFFERENT TYPES OF SHADERS SUPPORTED BY VULKAN
-------------------------------------------
VK_SHADER_STAGE_VERTEX_BIT = 0x00000001,
VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT = 0x00000002,
VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT = 0x00000004,
VK_SHADER_STAGE_GEOMETRY_BIT = 0x00000008,
VK_SHADER_STAGE_FRAGMENT_BIT = 0x00000010,
VK_SHADER_STAGE_COMPUTE_BIT = 0x00000020,
VK_SHADER_STAGE_ALL_GRAPHICS = 0x0000001F,
VK_SHADER_STAGE_ALL = 0x7FFFFFFF,
-------------------------------------------
VK_SHADER_STAGE_RAYGEN_BIT_KHR = 0x00000100,
VK_SHADER_STAGE_ANY_HIT_BIT_KHR = 0x00000200,
VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR = 0x00000400,
VK_SHADER_STAGE_MISS_BIT_KHR = 0x00000800,
VK_SHADER_STAGE_INTERSECTION_BIT_KHR = 0x00001000,
VK_SHADER_STAGE_CALLABLE_BIT_KHR = 0x00002000,
-------------------------------------------
VK_SHADER_STAGE_TASK_BIT_EXT = 0x00000040,
VK_SHADER_STAGE_MESH_BIT_EXT = 0x00000080,
*/


namespace fml {
	namespace fs = std::filesystem;

	/* ///// VERT SHADER INPUTS /////
	----+------------------------+----------------------------+-------------------------+
		|    Data Type           |        Frequency           |      Vulkan Mechanism    |
	----+------------------------+----------------------------+-------------------------+
		| Vertex Attributes      | Per-vertex                 | Vertex Buffers           |
	----+------------------------+----------------------------+-------------------------+
		| Uniform Buffers (UBOs) | Per-draw call              | UBOs                     |
	----+------------------------+----------------------------+-------------------------+
		| Push Constants         | Per-object                 | Push Constants           |
	----+------------------------+----------------------------+-------------------------+
		| Storage Buffers (SSBOs)| Dynamic                    | SSBOs                    |
	----+------------------------+----------------------------+-------------------------+
		| Descriptor Sets        | Per-draw call              | Descriptors              |
	----+------------------------+----------------------------+-------------------------+
		| Specialization Const.  | Pipeline creation          | Specialization Constants |
	----+------------------------+----------------------------+-------------------------+
		| Instance Attributes    | Per-instance               | Instanced Rendering      |
	----+------------------------+----------------------------+-------------------------+
	*/

	class vertexShader : public shader {
	public:
		vertexShader() = default;
		vertexShader(fs::path filepath, std::string entryPoint) : shader(filepath, entryPoint) {

		}

	public:
		// NOTE: input *attributes[] MUST NOT GO OUT OF SCOPE!
		void setVertexAttributes(VkVertexInputAttributeDescription* attributes, const uint32_t& attribCount);

		VkVertexInputAttributeDescription* getVertexAttributes() noexcept;

		const uint32_t& getVertexAttributesCount() noexcept;

		void setBindingDescription(const VkVertexInputBindingDescription& bindingDescription);

		const VkVertexInputBindingDescription& getBindingDescription() noexcept;

		VkShaderStageFlagBits getStageFlagBits() override;

		std::vector<VkDescriptorSetLayoutBinding>* getLayoutBindings() override;
	private:
		VkVertexInputAttributeDescription* _attributes = NULL;
		uint32_t _attribCount = 0;
		VkVertexInputBindingDescription _bindingDescription{};
	};
}

#endif // !FML_VERTEX_SHADER_HPP