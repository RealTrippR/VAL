#include <VAL/lib/system/pipelineCreateInfo.hpp>
#include <VAL/lib/graphics/shader.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

#ifndef NDEBUG 
#define VAL_VALIDATE_PUSH_DESCRIPTOR_EXT if (vkCmdPushDescriptorSetKHR == VK_NULL_HANDLE) {printf("VAL: ERROR: Attempted to use push descriptors, but the push descriptor extension (VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME) was not enabled in the physicalDeviceRequirements!\n");}
#else
#define VAL_VALIDATE_PUSH_DESCRIPTOR_EXT
#endif

namespace val {

	PFN_vkCmdPushDescriptorSetKHR vkCmdPushDescriptorSetKHR = VK_NULL_HANDLE;

	void pipelineCreateInfo_loadvkCmdPushDescriptorSetKHR(VkDevice device) {
		if (vkCmdPushDescriptorSetKHR == VK_NULL_HANDLE) {
			vkCmdPushDescriptorSetKHR = (PFN_vkCmdPushDescriptorSetKHR)vkGetDeviceProcAddr(device, "vkCmdPushDescriptorSetKHR");
		}
	}


	std::vector<UBO_Handle*> pipelineCreateInfo::getUniqueUBOs() const {
		std::vector<UBO_Handle*> UBO_Handles;
		for (shader* shdr : shaders) {
			for (auto& UBO_Write : shdr->_UBO_Handles) {
				for (auto& UBO_Hdl : UBO_Write.values) {

					UBO_Hdl->stageFlags |= shdr->getStageFlags();

					// if the handle is not already in the list of unique handles, add it to the list
					if (std::find(UBO_Handles.begin(), UBO_Handles.end(), UBO_Hdl) == UBO_Handles.end()) {
						UBO_Handles.push_back(UBO_Hdl);
					}

				}
			}
		}
		return UBO_Handles;
	}

	std::vector<pushConstantHandle*> pipelineCreateInfo::getUniquePushConstants() const {
		std::vector<pushConstantHandle*> PC_Handles;
		for (shader* shdr : shaders) {
			if (shdr->_pushConstant != NULL) {

				pushConstantHandle* PC_Hdl = shdr->_pushConstant;

				PC_Hdl->_stageFlags |= shdr->getStageFlags();

				// if the handle is not already in the list of unique handles, add it to the list
				if (std::find(PC_Handles.begin(), PC_Handles.end(), PC_Hdl) == PC_Handles.end()) {
					PC_Handles.push_back(PC_Hdl);
				}

			}
		}
		return PC_Handles;
	}

	std::vector<SSBO_Handle*> pipelineCreateInfo::getUniqueSSBOs() const {
		std::vector<SSBO_Handle*> SSBO_Handles;
		for (shader* shdr : shaders) {
			for (auto SSBO_Write : shdr->_SSBO_Handles) {
				for (SSBO_Handle* SSBO_HDL : SSBO_Write.values) {

					SSBO_HDL->_stageFlags |= shdr->getStageFlags();

					// if the handle is not already in the list of unique handles, add it to the list
					if (std::find(SSBO_Handles.begin(), SSBO_Handles.end(), SSBO_HDL) == SSBO_Handles.end()) {
						SSBO_Handles.push_back(SSBO_HDL);
					}

				}
			}
		}
		return SSBO_Handles;
	}

	std::vector<VkDescriptorSet> pipelineCreateInfo::getDescriptorSets(VAL_PROC& proc) const {
		return proc._descriptorSets[descriptorsIdx];
	}

	// std::vector<VkPipelineStageFlags> pipelineCreateInfo::getPipelineStages() {
	//	 std::vector<VkPipelineStageFlags> stages;

	//	 for (auto shdr : shaders) {
	//		 VkPipelineStageFlags pipelineStage = 0;
	//		 VkShaderStageFlags shaderStage = shdr->getStageFlags();
	//		 if (shaderStage & VK_SHADER_STAGE_VERTEX_BIT)
	//			 pipelineStage |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
	//		 if (shaderStage & VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT)
	//			 pipelineStage |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
	//		 if (shaderStage & VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
	//			 pipelineStage |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
	//		 if (shaderStage & VK_SHADER_STAGE_GEOMETRY_BIT)
	//			 pipelineStage |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
	//		 if (shaderStage & VK_SHADER_STAGE_FRAGMENT_BIT)
	//			 pipelineStage |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	//		 if (shaderStage & VK_SHADER_STAGE_COMPUTE_BIT)
	//			 pipelineStage |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	//		 if (shaderStage & VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT)
	//			 pipelineStage |= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
	//	     if (shaderStage & VK_SHADER_STAGE_ALL)
	//			 pipelineStage |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

	//		stages.push_back(pipelineStage);
	//	 }

	//	return stages;
	//};
	const std::vector<VkShaderStageFlags> pipelineCreateInfo::getShaderStages() const {
		std::vector<VkShaderStageFlags> stages;
		for (auto shdr : shaders) {
			stages.push_back(shdr->getStageFlags());
		}
		return stages;
	}


	void pipelineCreateInfo::pushDescriptor_SAMPLER(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, sampler& sampler) {
		VAL_VALIDATE_PUSH_DESCRIPTOR_EXT;

		VkDescriptorImageInfo imageInfo = sampler.getVkDescriptorImageInfo();

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstBinding = bindingIdx;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		write.pImageInfo = &(sampler.getVkDescriptorImageInfo());

		vkCmdPushDescriptorSetKHR(
			cmdBuffer,
			getBindPoint(),
			proc._pipelineLayouts[pipelineIdx],
			pushDescriptorsSetNo,
			write.descriptorCount,
			&write
		);
	}

	void pipelineCreateInfo::pushDescriptor_COMBINED_SAMPLER(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, sampler& sampler) {
		VAL_VALIDATE_PUSH_DESCRIPTOR_EXT;

		VkDescriptorImageInfo imageInfo = sampler.getVkDescriptorImageInfo();

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstBinding = bindingIdx;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.pImageInfo = &(sampler.getVkDescriptorImageInfo());

		vkCmdPushDescriptorSetKHR(
			cmdBuffer,
			getBindPoint(),
			proc._pipelineLayouts[pipelineIdx],
			pushDescriptorsSetNo,
			write.descriptorCount,
			&write
		);
	}


	void pipelineCreateInfo::pushDescriptor_SAMPLED_IMAGE(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, imageView& imgView) {
		VAL_VALIDATE_PUSH_DESCRIPTOR_EXT;

		VkDescriptorImageInfo imgInfo{ NULL, imgView.getImageView(),imgView.getImage().getLayout() };

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstBinding = bindingIdx;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		write.pImageInfo = &imgInfo;

		vkCmdPushDescriptorSetKHR(
			cmdBuffer,
			getBindPoint(),
			proc._pipelineLayouts[pipelineIdx],
			pushDescriptorsSetNo,
			write.descriptorCount,
			&write
		);
	}

	void pipelineCreateInfo::pushDescriptor_SAMPLED_IMAGE(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const uint16_t arrIdx, imageView& imgView) {
		VAL_VALIDATE_PUSH_DESCRIPTOR_EXT;

		VkDescriptorImageInfo imgInfo{ NULL, imgView.getImageView(),imgView.getImage().getLayout() };

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = VK_NULL_HANDLE;  // handled by vkCmdPushDescriptorSetKHR
		write.dstBinding = bindingIdx;
		write.dstArrayElement = arrIdx;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		write.pImageInfo = &imgInfo;

		vkCmdPushDescriptorSetKHR(
			cmdBuffer,
			getBindPoint(),
			proc._pipelineLayouts[pipelineIdx],
			pushDescriptorsSetNo,
			write.descriptorCount,
			&write
		);
	}

	void pipelineCreateInfo::pushDescriptor_STORAGE_IMAGE(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, imageView& imgView) {
		VAL_VALIDATE_PUSH_DESCRIPTOR_EXT;

		VkDescriptorImageInfo imageInfo;
		imageInfo.sampler = VK_NULL_HANDLE;
		imageInfo.imageView = imgView.getImageView();
		imageInfo.imageLayout = imgView.getImage().getLayout();

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstBinding = bindingIdx;
		write.dstArrayElement = 0;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		write.descriptorCount = 1;
		write.pImageInfo = &imageInfo;

		vkCmdPushDescriptorSetKHR(
			cmdBuffer,
			getBindPoint(),
			proc._pipelineLayouts[pipelineIdx],
			pushDescriptorsSetNo,
			write.descriptorCount,
			&write
		);
	}

	void pipelineCreateInfo::pushDescriptor_STORAGE_IMAGE(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const uint16_t arrIndex, imageView& imgView) {
		VAL_VALIDATE_PUSH_DESCRIPTOR_EXT;

		VkDescriptorImageInfo imageInfo;
		imageInfo.sampler = VK_NULL_HANDLE;
		imageInfo.imageView = imgView.getImageView();
		imageInfo.imageLayout = imgView.getImage().getLayout();

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstBinding = bindingIdx;
		write.dstArrayElement = arrIndex;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		write.descriptorCount = 1;
		write.pImageInfo = &imageInfo;

		vkCmdPushDescriptorSet(
			cmdBuffer,
			getBindPoint(),
			proc._pipelineLayouts[pipelineIdx],
			pushDescriptorsSetNo,
			write.descriptorCount,
			&write
		);
	}

	void pipelineCreateInfo::pushDescriptor_UNIFORM_BUFFER(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, UBO_Handle& ubo) {
		VAL_VALIDATE_PUSH_DESCRIPTOR_EXT;

		const VkDescriptorBufferInfo bufferInfo{ ubo.getBuffer(proc), ubo._offset, ubo._size };

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstBinding = bindingIdx;
		write.dstArrayElement = 0;
		write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write.descriptorCount = 1;
		write.pBufferInfo = &bufferInfo;

		vkCmdPushDescriptorSetKHR(
			cmdBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			proc._pipelineLayouts[pipelineIdx],
			pushDescriptorsSetNo,
			write.descriptorCount,
			&write
		);
	}
	void pipelineCreateInfo::pushDescriptor_UNIFORM_BUFFER(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const uint16_t arrIndex, UBO_Handle& ubo) {
		VAL_VALIDATE_PUSH_DESCRIPTOR_EXT;

		const VkDescriptorBufferInfo bufferInfo{ ubo.getBuffer(proc), ubo._offset, ubo._size };

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstBinding = bindingIdx;
		write.dstArrayElement = arrIndex;
		write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write.descriptorCount = 1;
		write.pBufferInfo = &bufferInfo;

		vkCmdPushDescriptorSetKHR(
			cmdBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			proc._pipelineLayouts[pipelineIdx],
			pushDescriptorsSetNo,
			write.descriptorCount,
			&write
		);
	}

	void pipelineCreateInfo::pushDescriptor_STORAGE_BUFFER(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, SSBO_Handle& ssbo) {
		VAL_VALIDATE_PUSH_DESCRIPTOR_EXT;

		const VkDescriptorBufferInfo bufferInfo{ ssbo.getBuffer(proc), 0, ssbo._size };

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = VK_NULL_HANDLE;  // handled by vkCmdPushDescriptorSetKHR
		write.dstBinding = bindingIdx;
		write.dstArrayElement = 0;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		write.descriptorCount = 1;
		write.pBufferInfo = &bufferInfo;

		vkCmdPushDescriptorSetKHR(
			cmdBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			proc._pipelineLayouts[pipelineIdx],
			descriptorsIdx,
			write.descriptorCount,
			&write
		);
	}
	void pipelineCreateInfo::pushDescriptor_STORAGE_BUFFER(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const uint16_t arrIndex, SSBO_Handle& ssbo) {
		VAL_VALIDATE_PUSH_DESCRIPTOR_EXT;

		const VkDescriptorBufferInfo bufferInfo{ ssbo.getBuffer(proc), 0, ssbo._size };

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = VK_NULL_HANDLE;  // handled by vkCmdPushDescriptorSetKHR
		write.dstBinding = bindingIdx;
		write.dstArrayElement = arrIndex;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		write.descriptorCount = 1;
		write.pBufferInfo = &bufferInfo;

		vkCmdPushDescriptorSetKHR(
			cmdBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			proc._pipelineLayouts[pipelineIdx],
			descriptorsIdx,
			write.descriptorCount,
			&write
		);
	}

	bool pipelineCreateInfo::hasPushDescriptorLayout() {
		return !(pushDescriptorsSetNo == UINT32_MAX);
	}

}