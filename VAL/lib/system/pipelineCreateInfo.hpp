#ifndef FML_PIPELINE_CREATE_INFO_HPP
#define FML_PIPELINE_CREATE_INFO_HPP

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/renderPass.hpp>
#include <VAL/lib/system/UBO_Handle.hpp>
#include <VAL/lib/system/pushConstantHandle.hpp>
#include <VAL/lib/system/SSBO_Handle.hpp>


#include <VAL/lib/system/sampler.hpp>

#include <vector>
#include <algorithm>

namespace val {
	class shader; // forward declaration
	class imageView; // forward declaration

	void pipelineCreateInfo_loadvkCmdPushDescriptorSetKHR(VkDevice device);

	class pipelineCreateInfo {
	public:

		inline VkPipelineBindPoint getBindPoint() {  return _bindPoint; };

		virtual std::vector<UBO_Handle*> getUniqueUBOs() const;

		virtual std::vector<pushConstantHandle*> getUniquePushConstants() const;

		virtual std::vector<SSBO_Handle*> getUniqueSSBOs() const;

		virtual std::vector<VkDescriptorSet> getDescriptorSets(VAL_PROC& proc) const;

		//std::vector<VkPipelineStageFlags> getPipelineStages();

		virtual const std::vector<VkShaderStageFlags> getShaderStages()const;

	public:
		void pushDescriptor_SAMPLER(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, sampler& sampler);

		void pushDescriptor_COMBINED_SAMPLER(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, sampler& sampler);

		void pushDescriptor_SAMPLED_IMAGE(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, imageView& imgView);
		void pushDescriptor_SAMPLED_IMAGE(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const uint16_t arrIdx, imageView& imgView);

		void pushDescriptor_STORAGE_IMAGE(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, imageView& imgView);
		void pushDescriptor_STORAGE_IMAGE(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const uint16_t arrIndex, imageView& imgView);

		//void pushDescriptor_UNIFORM_TEXEL_BUFFER(VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const TexelUBO_Hndle& ubo);
		//void pushDescriptor_STORAGE_TEXEL_BUFFER(VkCommandBuffer cmdBuffer, const uint16_t bindingIdx);
		void pushDescriptor_UNIFORM_BUFFER(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, UBO_Handle& ubo);
		void pushDescriptor_UNIFORM_BUFFER(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const uint16_t arrIndex, UBO_Handle& ubo);

		void pushDescriptor_STORAGE_BUFFER(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, SSBO_Handle& ssbo);
		void pushDescriptor_STORAGE_BUFFER(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const uint16_t arrIndex, SSBO_Handle& ssbo);

		// returns true if the pipeline has a push descriptor layout, returns false if otherwise.
		bool hasPushDescriptorLayout();

	public:
		std::vector<shader*> shaders;
		uint32_t pipelineIdx = 0u;
		uint32_t descriptorsIdx = 0u; // index of descriptor sets and layouts
		uint32_t pushDescriptorsSetNo = UINT32_MAX; // may point to an invalid value, represented by UINT32_MAX, be careful
		VkPipelineBindPoint _bindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;
	};
}

#endif // !FML_COMPUTE_PIPELINE_CREATE_INFO_HPP