#include <VAL/lib/system/pipelineBase.hpp>
#include <VAL/lib/graphics/shader.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/imageView.hpp>

#ifndef NDEBUG 
#define VAL_VALIDATE_PUSH_DESCRIPTOR_EXT if (vkCmdPushDescriptorSetKHR == VK_NULL_HANDLE) {val::dbg::printError("Attempted to use push descriptors, but the push descriptor extension (VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME) was not enabled in the physicalDeviceRequirements!\n");}
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


	std::vector<UBO_Handle*> PipelineBase::getUniqueUBOs() const {
		if (descriptorSheet==NULL) {
			return { };
		}
		std::vector<UBO_Handle*> UBO_Handles;
		for (const DescriptorSheetElement& element : descriptorSheet->getSheetElements()) 
		{
			if (element.getType() == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
			{
				// if the handle is not already in the list of unique handles, add it to the list
				if (std::find(UBO_Handles.begin(), UBO_Handles.end(), element.getValObject()) == UBO_Handles.end()) {
					UBO_Handles.push_back((UBO_Handle*)element.getValObject());
				}
			}
		}
		return UBO_Handles;
	}

	std::vector<pushConstantHandle*> PipelineBase::getUniquePushConstants() const {
		std::vector<pushConstantHandle*> PC_Handles;
		for (Shader* shdr : shaders) {
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

	std::vector<SSBO_Handle*> PipelineBase::getUniqueSSBOs() const {
		if (descriptorSheet==NULL){
			return {};
		}
		std::vector<SSBO_Handle*> SSBO_Handles;
		for (const DescriptorSheetElement& element : descriptorSheet->getSheetElements())
		{
			if (element.getType() == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
			{
				// if the handle is not already in the list of unique handles, add it to the list
				if (std::find(SSBO_Handles.begin(), SSBO_Handles.end(), element.getValObject()) == SSBO_Handles.end()) {
					SSBO_Handles.push_back((SSBO_Handle*)element.getValObject());
				}
			}
		}
		return SSBO_Handles;
	}

	tiny_vector<VkDescriptorSet>& PipelineBase::getDescriptorSets() const {
		return descriptorSheet->getVkDescriptorSets();
	}

	VkPipelineLayout PipelineBase::getPipelineLayout(ValProc& proc) const
	{
		switch (_bindPoint) {
		case VK_PIPELINE_BIND_POINT_GRAPHICS:
			return proc._graphicsPipelineLayouts[pipelineIdx];
		case VK_PIPELINE_BIND_POINT_COMPUTE:
			return proc._computePipelineLayouts[pipelineIdx];
		}
		dbg::printWarning("PipelineBase::getPipelineLayout: Attempted to layout, but Pipeline @ %p does not have a valid bindpoint (value: %lu), thus VK_NULL_HANDLE was returned.", this, _bindPoint);
		return VK_NULL_HANDLE;
	}

	VAL_RETURN_CODE PipelineBase::allocateDescriptorSets(ValProc& proc)
	{
		if (!descriptorSheet) {
			return VAL_FAILURE;
		}
		const uint32_t maxSetCount = descriptorSheet->getMaxSetCount();
		descriptorSheet->getVkDescriptorSets().resize(maxSetCount);
		tiny_vector<VkDescriptorSetLayout> setLayoutCopies(maxSetCount, getDescriptorSetLayout(proc));

		return descriptorSheet->allocateSets(
			proc.getVkLogicalDevice(),
			descriptorSheet->getVkDescriptorSets().data(),
			setLayoutCopies.data(),
			maxSetCount,
			proc.getVkDescriptorPool()
		);
	}

	void PipelineBase::writeDescriptorSets(ValProc& proc)
	{
		for (uint8_t i = 0; i < descriptorSheet->getVkDescriptorSets().size(); ++i) {
			descriptorSheet->updateAndWriteDescriptors(
				proc.getVkLogicalDevice(),
				descriptorSheet->getVkDescriptorSets().data()[i]
			);
		}
	}

	VAL_RETURN_CODE PipelineBase::allocateAndWriteDescriptorSets(ValProc& proc)
	{
		if (!descriptorSheet) {
			return VAL_FAILURE;
		}
		
		const uint32_t maxSetCount = descriptorSheet->getMaxSetCount();
		descriptorSheet->getVkDescriptorSets().resize(maxSetCount);
		tiny_vector<VkDescriptorSetLayout> setLayoutCopies(maxSetCount, getDescriptorSetLayout(proc));
		
		const VAL_RETURN_CODE allocAndWriteRes = descriptorSheet->allocateAndWriteSets(
			proc.getVkLogicalDevice(), 
			descriptorSheet->getVkDescriptorSets().data(),
			setLayoutCopies.data(),
			maxSetCount, 
			proc.getVkDescriptorPool()
		);

		return allocAndWriteRes;
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
	const std::vector<VkShaderStageFlags> PipelineBase::getShaderStages() const {
		std::vector<VkShaderStageFlags> stages;
		for (auto shdr : shaders) {
			stages.push_back(shdr->getStageFlags());
		}
		return stages;
	}

	int32_t PipelineBase::getShaderIndexOfShaderInPipeline(Shader* shdr)
	{
		for (uint32_t i = 0; i < shaders.size(); ++i)
		{
			if (shaders[i] == shdr)
			{
				return i;
			}
		}
		return -1;
	}

	void PipelineBase::pushDescriptor_SAMPLER(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, Sampler& sampler) {
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
			proc._graphicsPipelineLayouts[pipelineIdx],
			pushDescriptorsSetNo,
			write.descriptorCount,
			&write
		);
	}

	void PipelineBase::pushDescriptor_COMBINED_SAMPLER(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, Sampler& sampler) {
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
			proc._graphicsPipelineLayouts[pipelineIdx],
			pushDescriptorsSetNo,
			write.descriptorCount,
			&write
		);
	}


	void PipelineBase::pushDescriptor_SAMPLED_IMAGE(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, ImageView& imgView)
	{
		VAL_VALIDATE_PUSH_DESCRIPTOR_EXT;

		VkDescriptorImageInfo imgInfo{ NULL, imgView.getImageView(), imgView.getLayout()};

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
			proc._graphicsPipelineLayouts[pipelineIdx],
			pushDescriptorsSetNo,
			write.descriptorCount,
			&write
		);
	}

	void PipelineBase::pushDescriptor_SAMPLED_IMAGE(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const uint16_t arrIdx, ImageView& imgView)
	{
		VAL_VALIDATE_PUSH_DESCRIPTOR_EXT;

		VkDescriptorImageInfo imgInfo{ NULL, imgView.getImageView(), imgView.getLayout()};

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
			proc._graphicsPipelineLayouts[pipelineIdx],
			pushDescriptorsSetNo,
			write.descriptorCount,
			&write
		);
	}

	void PipelineBase::pushDescriptor_STORAGE_IMAGE(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, ImageView& imgView)
	{
		VAL_VALIDATE_PUSH_DESCRIPTOR_EXT;

		VkDescriptorImageInfo imageInfo;
		imageInfo.sampler = VK_NULL_HANDLE;
		imageInfo.imageView = imgView.getImageView();
		imageInfo.imageLayout = imgView.getLayout();

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
			proc._graphicsPipelineLayouts[pipelineIdx],
			pushDescriptorsSetNo,
			write.descriptorCount,
			&write
		);
	}

	void PipelineBase::pushDescriptor_STORAGE_IMAGE(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const uint16_t arrIndex, ImageView& imgView)
	{
		VAL_VALIDATE_PUSH_DESCRIPTOR_EXT;

		VkDescriptorImageInfo imageInfo;
		imageInfo.sampler = VK_NULL_HANDLE;
		imageInfo.imageView = imgView.getImageView();
		imageInfo.imageLayout = imgView.getLayout();

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
			proc._graphicsPipelineLayouts[pipelineIdx],
			pushDescriptorsSetNo,
			write.descriptorCount,
			&write
		);
	}

	void PipelineBase::pushDescriptor_UNIFORM_BUFFER(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, UBO_Handle& ubo) {
		VAL_VALIDATE_PUSH_DESCRIPTOR_EXT;

		const VkDescriptorBufferInfo bufferInfo{ ubo.getBuffer(), ubo._offset, ubo._size };

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
			proc._graphicsPipelineLayouts[pipelineIdx],
			pushDescriptorsSetNo,
			write.descriptorCount,
			&write
		);
	}
	void PipelineBase::pushDescriptor_UNIFORM_BUFFER(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const uint16_t arrIndex, UBO_Handle& ubo) {
		VAL_VALIDATE_PUSH_DESCRIPTOR_EXT;

		const VkDescriptorBufferInfo bufferInfo{ ubo.getBuffer(), ubo._offset, ubo._size };

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
			proc._graphicsPipelineLayouts[pipelineIdx],
			pushDescriptorsSetNo,
			write.descriptorCount,
			&write
		);
	}

	void PipelineBase::pushDescriptor_STORAGE_BUFFER(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, SSBO_Handle& ssbo) {
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
			proc._graphicsPipelineLayouts[pipelineIdx],
			descriptorsIdx,
			write.descriptorCount,
			&write
		);
	}
	void PipelineBase::pushDescriptor_STORAGE_BUFFER(ValProc& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const uint16_t arrIndex, SSBO_Handle& ssbo)
	{
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
			proc._graphicsPipelineLayouts[pipelineIdx],
			descriptorsIdx,
			write.descriptorCount,
			&write
		);
	}

	bool PipelineBase::hasPushDescriptorLayout() {
		return !(pushDescriptorsSetNo == UINT32_MAX);
	}
	
	void PipelineBase::setDescriptorSheet(DescriptorSheet* sheet)
	{
		descriptorSheet = sheet;
	}

	DescriptorSheet* PipelineBase::getDescriptorSheet() const
	{
		return descriptorSheet;
	}

	void PipelineBase::setShaders(const tiny_vector<Shader*>& shaders)
	{
		this->shaders = shaders;
	}

	tiny_vector<Shader*> PipelineBase::getShaders() const
	{
		return shaders;
	}

	VkDescriptorSetLayout PipelineBase::getDescriptorSetLayout(ValProc& proc) const
	{
#ifndef NDEBUG
		if (descriptorsIdx >= proc._descriptorSetLayouts.size())
		{
			dbg::printError("pipelineCreateInfo::getDescriptorSetLayout: Invalid descriptorsIdx in pipelineCreateInfo @ %p, it exceeds proc._descriptorSetLayouts.size().",this);
			return VK_NULL_HANDLE;
		}
#endif // !NDEBUG

		return proc._descriptorSetLayouts[descriptorsIdx];
	}

	void PipelineBase::setDynamicStates(const tiny_vector<DYNAMIC_STATE>& dynamicStates)
	{
		_dynamicStates = dynamicStates;
	}

	const tiny_vector<DYNAMIC_STATE>& PipelineBase::getDynamicStates() const
	{
		return _dynamicStates;
	}

}