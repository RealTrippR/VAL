#ifndef VAL_PIPELINE_BASE_HPP
#define VAL_PIPELINE_BASE_HPP

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/renderPass.hpp>
#include <VAL/lib/system/UBO_Handle.hpp>
#include <VAL/lib/system/pushConstantHandle.hpp>
#include <VAL/lib/system/SSBO_Handle.hpp>
#include <VAL/lib/descriptorSheets/descriptorSheet.hpp>
#include <VAL/lib/descriptorSheets/pushDescriptorSheet.hpp>

#include <VAL/lib/system/sampler.hpp>

#include <vector>
#include <algorithm>

namespace val {
	class Shader; // forward declaration
	class ImageView; // forward declaration

	void pipelineCreateInfo_loadvkCmdPushDescriptorSetKHR(VkDevice device);

	class PipelineBase {
	public:

		inline VkPipelineBindPoint getBindPoint() {  return _bindPoint; };

		virtual std::vector<UBO_Handle*> getUniqueUBOs() const;

		virtual std::vector<SSBO_Handle*> getUniqueSSBOs() const;

		virtual tiny_vector<VkDescriptorSet>& getDescriptorSets() const;

		void setPushConstants(const tiny_vector<pushConstantHandle*>&);

		const tiny_vector<pushConstantHandle*>& getPushConstants() const;

		VkPipelineLayout getPipelineLayout(ValProc& proc) const;

		VAL_RETURN_CODE allocateDescriptorSets(ValProc& proc);

		void writeDescriptorSets(ValProc& proc);

		VAL_RETURN_CODE allocateAndWriteDescriptorSets(ValProc& proc);

		virtual const std::vector<VkShaderStageFlags> getShaderStages()const;


		// returns UINT32MAX if not found, otherwise returns the index as a uint32_t
		int32_t getShaderIndexOfShaderInPipeline(Shader* shdr);

	public:
		void pushDescriptor_SAMPLER(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, Sampler& sampler);

		void pushDescriptor_COMBINED_SAMPLER(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, Sampler& sampler);

		void pushDescriptor_SAMPLED_IMAGE(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, ImageView& imgView);
		void pushDescriptor_SAMPLED_IMAGE(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const uint16_t arrIdx, ImageView& imgView);

		void pushDescriptor_STORAGE_IMAGE(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, ImageView& imgView);
		void pushDescriptor_STORAGE_IMAGE(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const uint16_t arrIndex, ImageView& imgView);

		//void pushDescriptor_UNIFORM_TEXEL_BUFFER(VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const TexelUBO_Hndle& ubo);
		//void pushDescriptor_STORAGE_TEXEL_BUFFER(VkCommandBuffer cmdBuffer, const uint16_t bindingIdx);
		void pushDescriptor_UNIFORM_BUFFER(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, UBO_Handle& ubo);
		void pushDescriptor_UNIFORM_BUFFER(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const uint16_t arrIndex, UBO_Handle& ubo);

		void pushDescriptor_STORAGE_BUFFER(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, SSBO_Handle& ssbo);
		void pushDescriptor_STORAGE_BUFFER(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const uint16_t arrIndex, SSBO_Handle& ssbo);

		// returns true if the pipeline has a push descriptor layout, returns false if otherwise.
		bool hasPushDescriptorLayout();

		void setDescriptorSheet(DescriptorSheet* descSheet);

		DescriptorSheet* getDescriptorSheet() const;

		void setPushDescriptorSheet(PushDescriptorSheet* descSheet);

		PushDescriptorSheet* getPushDescriptorSheet() const;
		 
		void setShaders(const tiny_vector<Shader*>& shaders);

		tiny_vector<Shader*> getShaders() const;

		VkDescriptorSetLayout getDescriptorSetLayout(ValProc& proc) const;

		void setDynamicStates(const tiny_vector<DYNAMIC_STATE>& dynamicStates);

		const tiny_vector<DYNAMIC_STATE>& getDynamicStates() const;

	public:
		tiny_vector<pushConstantHandle*> _pushConstants;
		// VAL::DYNAMIC_STATE maps directly to VkDynamicState
		tiny_vector<DYNAMIC_STATE> _dynamicStates;

		DescriptorSheet* descriptorSheet = NULL;
		PushDescriptorSheet* pushDescriptorSheet = NULL;

		tiny_vector<Shader*> shaders;
		uint32_t pipelineIdx = 0u;
		uint32_t descriptorsIdx = 0u; // index of descriptor sets and layouts
		uint32_t pushDescriptorsSetNo = UINT32_MAX; // may point to an invalid value, represented by UINT32_MAX, be careful
		VkPipelineBindPoint _bindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;
	};
}

#endif // !VAL_PIPELINE_BASE_HPP