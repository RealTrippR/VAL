#ifndef VAL_SHADER_HPP
#define VAL_SHADER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/UBO_Handle.hpp>
#include <VAL/lib/system/pushConstantHandle.hpp>
#include <VAL/lib/system/SSBO_Handle.hpp>
#include <VAL/lib/system/specializationConstant.hpp>
#include <VAL/lib/system/sampler.hpp>

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

		// change to allow for multiple image samplers (only one allowed per image view)
		void setImageSamplers(const std::vector< std::pair<VkSamplerCreateInfo, uint16_t>> sampler);

		void createImageSamplers(VAL_PROC* proc);

		const fs::path& getFilepath() noexcept;

		const std::vector<char>& getByteCode() noexcept;

		void setStageFlags(const VkShaderStageFlags& stageFlags);

		VkShaderStageFlags getStageFlags() noexcept;

		virtual std::vector<VkDescriptorSetLayoutBinding>* getLayoutBindings() noexcept;

		virtual std::vector<VkWriteDescriptorSet>* getDescriptorWrites();

		// outer vector is for each frame in flight, second is for the buffer info for that frame
		virtual std::vector<std::vector<VkDescriptorBufferInfo>>* getDescriptorBufferInfos(VAL_PROC& proc);

		void setVertexAttributes(const std::vector<VkVertexInputAttributeDescription>& attributes);

		const std::vector<VkVertexInputAttributeDescription>& getVertexAttributes() noexcept;

		void setBindingDescriptions(const std::vector<VkVertexInputBindingDescription>& bindingDescriptions);

		const std::vector<VkVertexInputBindingDescription>& getBindingDescriptions() noexcept;

		void setPushConstants(const std::vector<std::pair<pushConstantHandle*, uint16_t>>& pushConstants);

		const std::vector<std::pair<pushConstantHandle*, uint16_t>>& getPushConstants() noexcept;

	public:
		void changeImageView(VAL_PROC& proc, const pipelineCreateInfo& pipeline, const uint32_t& imgViewIndex, val::imageView newImgView);

		void changeImageSampler(VAL_PROC& proc, const uint32_t imgSamplerindex, VkSampler newSampler);

	public:
		VkShaderStageFlags _shaderStageFlags;

		std::string _entryPoint = "main";
		std::vector<char> _byteCode;
		fs::path _filepath;

		std::vector<std::pair<specializationConstant*/*Constant*/, uint16_t/*constant_id*/>> _specializationConstants;
		std::vector<std::pair<pushConstantHandle*, uint16_t>> _pushConstants;
		std::vector< std::pair<UBO_Handle*, uint16_t>> _UBO_Handles;
		std::vector<std::pair<SSBO_Handle*, uint16_t>> _SSBO_Handles;
		std::vector<std::pair<val::sampler, uint16_t>> _imageSamplers;
		std::vector<VkImageView*> _imageViews;

		std::vector< VkDescriptorSetLayoutBinding> _layoutBindings;
		std::vector<VkWriteDescriptorSet> _descriptorWrites; // stored as part of the class for optimization (avoids excess copying), as well as scope
		// outer vector is for each frame in flight, second is for the buffer info for that frame
		std::vector<std::vector<VkDescriptorBufferInfo>> _descriptorWriteBufferInfos; // stored as part of the class for optimization (avoids excess copying), as well as scope

		std::vector<VkVertexInputAttributeDescription> _attributes;
		std::vector< VkVertexInputBindingDescription> _bindings;
		/*VkVertexInputAttributeDescription* _attributes = NULL;
		uint32_t _attribCount = 0;
		VkVertexInputBindingDescription* _bindingDescription = NULL;
		uint32_t _bindingCount = 0;*/
	};
}

#endif // !VAL_SHADER_HPP