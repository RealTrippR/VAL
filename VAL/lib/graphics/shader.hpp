#ifndef VAL_SHADER_HPP
#define VAL_SHADER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/UBO_Handle.hpp>
#include <VAL/lib/system/pushConstantHandle.hpp>
#include <VAL/lib/system/SSBO_Handle.hpp>

#include <optional>
#include <string.h>
#include <stdexcept>
#include <filesystem>

/*THE DIFFERENT TYPES OF SHADERS SUPPORTED BY VULKAN
VK_SHADER_STAGE_VERTEX_BIT = 0x00000001,
VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT = 0x00000002,
VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT = 0x00000004,
VK_SHADER_STAGE_GEOMETRY_BIT = 0x00000008,
VK_SHADER_STAGE_FRAGMENT_BIT = 0x00000010,
VK_SHADER_STAGE_COMPUTE_BIT = 0x00000020,
VK_SHADER_STAGE_ALL_GRAPHICS = 0x0000001F,
VK_SHADER_STAGE_ALL = 0x7FFFFFFF,

// And raytracing....

*/

namespace val {
	class VAL_PROC; // forward declaration
	namespace fs = std::filesystem;

	class shader {
	public:

		shader() = default;

		shader(fs::path filepath, VkShaderStageFlags shaderStageFlags, std::string entryPoint) {
			loadFromFile(filepath);
			_entryPoint = entryPoint;
			_shaderStageFlags = shaderStageFlags;
		}

	public:
		// returns true if the shader was succesfully loaded
		bool loadFromFile(fs::path filepath);

		void setEntryPoint(const std::string& entryPoint);

		const std::string& getEntryPoint();

		//void setUniformBufferData(void* UBO, const size_t& sizeOfUBO, const uint32_t UBO_Count);

		void setUniformBuffers(const std::vector<UBO_Handle*> UBOs);

		// change to allow for multiple image samplers (only one allowed per image view)
		void setImageSamplers(const std::vector<VkSamplerCreateInfo> sampler);

		void createImageSamplers(VAL_PROC* procFML);

		// change to allow for multiple image views (only one allowed per image sampler)
		void setImageView(const std::vector<VkImageView*> imageView);

		//void setPushConstants();
		const fs::path& getFilepath() noexcept;

		const std::vector<char>& getByteCode() noexcept;

		void setStageFlags(const VkShaderStageFlags& stageFlags);

		VkShaderStageFlags getStageFlags() noexcept;

		virtual std::vector<VkDescriptorSetLayoutBinding>* getLayoutBindings() noexcept;

		virtual std::vector<VkWriteDescriptorSet>* getDescriptorWrites() noexcept;

		void setVertexAttributes(VkVertexInputAttributeDescription* attributes, const uint32_t& attribCount);

		VkVertexInputAttributeDescription* getVertexAttributes() noexcept;

		void setBindingDescription(const VkVertexInputBindingDescription& bindingDescription);

		const VkVertexInputBindingDescription& getBindingDescription() noexcept;

		const uint32_t& getVertexAttributesCount() noexcept;

		void setPushConstants(const std::vector<pushConstantHandle*>& pushConstants);

		const std::vector<pushConstantHandle*>& getPushConstants() noexcept;

	public:
		VkShaderStageFlags _shaderStageFlags;

		std::string _entryPoint = "main";
		std::vector<char> _byteCode;
		fs::path _filepath;

		std::vector< std::pair<UBO_Handle*, uint16_t>> _UBO_Handles;
		std::vector<std::pair<pushConstantHandle*, uint16_t>> _pushConstants;
		std::vector<std::pair<SSBO_Handle*, uint16_t>> _SSBO_Handles;
		std::optional<std::vector<VkSamplerCreateInfo>> _imageSamplersCreateInfos;
		std::vector<std::pair<VkSampler*, uint16_t>> _imageSamplers;
		std::vector<std::pair<VkImageView*, uint16_t>> _imageViews;

		std::vector< VkDescriptorSetLayoutBinding> _layoutBindings;
		std::vector<VkWriteDescriptorSet> _descriptorWrites; // stored as part of the class for optimization (avoids excess copying)

		VkVertexInputAttributeDescription* _attributes = NULL;
		uint32_t _attribCount = 0;
		VkVertexInputBindingDescription _bindingDescription{};
	};
}

#endif // !VAL_SHADER_HPP