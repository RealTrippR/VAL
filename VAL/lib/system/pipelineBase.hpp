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

		virtual const std::vector<VkShaderStageFlags> getShaderStages()const;

		// returns UINT32MAX if not found, otherwise returns the index as a uint32_t
		int32_t getShaderIndexOfShaderInPipeline(Shader* shdr);

	public:

		void setDescriptorSheet(DescriptorSheet* descSheet);

		DescriptorSheet* getDescriptorSheet() const;

		void setPushDescriptorSheet(PushDescriptorSheet* descSheet);

		PushDescriptorSheet* getPushDescriptorSheet() const;
		 
		void setShaders(const tiny_vector<Shader*>& shaders);

		tiny_vector<Shader*> getShaders() const;

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
		VkPipelineBindPoint _bindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;
	};
}

#endif // !VAL_PIPELINE_BASE_HPP