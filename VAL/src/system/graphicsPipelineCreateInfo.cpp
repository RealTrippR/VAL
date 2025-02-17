#include <VAL/lib/system/graphicsPipelineCreateInfo.hpp>
#include <VAL/lib/graphics/shader.hpp>

namespace val {
	std::vector<UBO_Handle*> graphicsPipelineCreateInfo::getUniqueUBOs() {
		std::vector<UBO_Handle*> UBO_Handles;
		for (shader* shdr : shaders) {
			for (UBO_Handle* UBO_Hdl : shdr->_UBOs) {
				if (std::find(UBO_Handles.begin(), UBO_Handles.end(), UBO_Hdl) == UBO_Handles.end()) {
					UBO_Hdl->stageFlags = VkShaderStageFlagBits(UBO_Hdl->stageFlags | shdr->getStageFlags());
					UBO_Handles.push_back(UBO_Hdl);
				}
			}

		}
		return UBO_Handles;
	}

	std::vector<pushConstantHandle*> graphicsPipelineCreateInfo::getUniquePushConstants() {
		std::vector<pushConstantHandle*> PC_Handles;
		for (shader* shdr : shaders) {
			for (pushConstantHandle* PC_Hdl : shdr->_pushConstants) {
				if (std::find(PC_Handles.begin(), PC_Handles.end(), PC_Hdl) == PC_Handles.end()) {
					PC_Hdl->_stageFlags = PC_Hdl->_stageFlags | shdr->getStageFlags();
					PC_Handles.push_back(PC_Hdl);
				}
			}

		}
		return PC_Handles;
	}

	std::vector<SSBO_Handle*> graphicsPipelineCreateInfo::getUniqueSSBOs() {
		std::vector<SSBO_Handle*> SSBO_Handles;
		for (shader* shdr : shaders) {
			for (SSBO_Handle* SSBO_HDL : shdr->_SSBO_Handles) {
				if (std::find(SSBO_Handles.begin(), SSBO_Handles.end(), SSBO_HDL) == SSBO_Handles.end()) {
					SSBO_HDL->_stageFlags = SSBO_HDL->_stageFlags | shdr->getStageFlags();
					SSBO_Handles.push_back(SSBO_HDL);
				}
			}

		}
		return SSBO_Handles;
	}
}