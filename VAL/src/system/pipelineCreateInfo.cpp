#include <VAL/lib/system/pipelineCreateInfo.hpp>
#include <VAL/lib/graphics/shader.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	std::vector<UBO_Handle*> pipelineCreateInfo::getUniqueUBOs() {
		std::vector<UBO_Handle*> UBO_Handles;
		for (shader* shdr : shaders) {
			for (auto UBO_PAIR : shdr->_UBO_Handles) {
				UBO_Handle* UBO_Hdl = UBO_PAIR.first;
				if (std::find(UBO_Handles.begin(), UBO_Handles.end(), UBO_Hdl) == UBO_Handles.end()) {
					UBO_Hdl->stageFlags = VkShaderStageFlagBits(UBO_Hdl->stageFlags | shdr->getStageFlags());
					UBO_Handles.push_back(UBO_Hdl);
				}
			}
		}
		return UBO_Handles;
	}

	std::vector<pushConstantHandle*> pipelineCreateInfo::getUniquePushConstants() {
		std::vector<pushConstantHandle*> PC_Handles;
		for (shader* shdr : shaders) {
			for (auto PC_PAIR : shdr->_pushConstants) {
				pushConstantHandle* PC_Hdl = PC_PAIR.first;
				if (std::find(PC_Handles.begin(), PC_Handles.end(), PC_Hdl) == PC_Handles.end()) {
					PC_Hdl->_stageFlags = PC_Hdl->_stageFlags | shdr->getStageFlags();
					PC_Handles.push_back(PC_Hdl);
				}
			}
		}
		return PC_Handles;
	}

	std::vector<SSBO_Handle*> pipelineCreateInfo::getUniqueSSBOs() {
		std::vector<SSBO_Handle*> SSBO_Handles;
		for (shader* shdr : shaders) {
			for (auto SSBO_PAIR : shdr->_SSBO_Handles) {
				SSBO_Handle* SSBO_HDL = SSBO_PAIR.first;
				if (std::find(SSBO_Handles.begin(), SSBO_Handles.end(), SSBO_HDL) == SSBO_Handles.end()) {
					SSBO_HDL->_stageFlags = SSBO_HDL->_stageFlags | shdr->getStageFlags();
					SSBO_Handles.push_back(SSBO_HDL);
				}
			}
		}
		return SSBO_Handles;
	}

	std::vector<VkDescriptorSet> pipelineCreateInfo::getDescriptorSets(VAL_PROC& proc) {
		return proc._descriptorSets[pipelineIdx];
	}

	 std::vector<VkPipelineStageFlags> pipelineCreateInfo::getPipelineShaderStages() {
		 std::vector<VkPipelineStageFlags> stages;

		 for (auto shdr : shaders) {
			 VkPipelineStageFlags pipelineStage = 0;
			 VkShaderStageFlags shaderStage = shdr->getStageFlags();
			 if (shaderStage & VK_SHADER_STAGE_VERTEX_BIT)
				 pipelineStage |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
			 if (shaderStage & VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT)
				 pipelineStage |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
			 if (shaderStage & VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
				 pipelineStage |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
			 if (shaderStage & VK_SHADER_STAGE_GEOMETRY_BIT)
				 pipelineStage |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
			 if (shaderStage & VK_SHADER_STAGE_FRAGMENT_BIT)
				 pipelineStage |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			 if (shaderStage & VK_SHADER_STAGE_COMPUTE_BIT)
				 pipelineStage |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			 if (shaderStage & VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT)
				 pipelineStage |= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		     if (shaderStage & VK_SHADER_STAGE_ALL)
				 pipelineStage |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

			stages.push_back(pipelineStage);
		 }

		return stages;
	};
	const std::vector<VkShaderStageFlags> pipelineCreateInfo::getShaderStages() {
		std::vector<VkShaderStageFlags> stages;
		for (auto shdr : shaders) {
			stages.push_back(shdr->getStageFlags());
		}
		return stages;
	}
}