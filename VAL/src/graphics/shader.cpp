#include <VAL/lib/graphics/shader.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	bool shader::loadFromFile(fs::path filepath) {
		_filepath = fs::absolute(filepath);
#ifndef NDEBUG

		if (!std::filesystem::exists(filepath)) {
			printf("VAL: WARNING: Attempted to load shader from invalid filepath: %ws\n", filepath.c_str());
			return false;
		}

#endif // !NDEBUG

		if (readByteFile(_filepath.string(), &_byteCode)) {
			return true;
		}
#ifndef  NDEBUG
		else {
			printf("\nVAL: FAILED TO READ FILE FROM DISK: %ws\n", filepath.c_str());
		}
#endif // ! NDEBUG
	}

	void shader::setEntryPoint(const std::string& entryPoint) {
		_entryPoint = entryPoint;
	}

	const std::string& shader::getEntryPoint() {
		return _entryPoint;
	}


	void shader::setImageSamplers(std::vector<descriptorBinding<val::sampler*>> samplerInfo) {
		_imageSamplers = samplerInfo;
		#ifndef NDEBUG
		// validate the samplerInfo. Samplers cannot be binded as arrays.
		for (auto& samplerWrite : samplerInfo) {
			if (samplerWrite.values.size() > 1) {
				printf("VAL: ERROR: Sampler descriptor writes can not be represented as arrays; the size of samplerWrite.values cannot be greater than 1!\n");
				throw std::runtime_error("VAL: ERROR: Sampler descriptor writes can not be represented as arrays; the size of samplerWrite.values cannot be greater than 1!");
			}
		}
		#endif // !NDEBUG

	}

	void shader::createImageSamplers(VAL_PROC* proc) {
		for (int i = 0; i < _imageSamplers.size(); ++i) {
#ifndef NDEBUG
			if (_imageSamplers[i].values.size() > 1) {
				printf("VAL: ERROR: Image samplers cannot be bound as an array; the values property cannot have a size above 1!");
				throw std::runtime_error("ERROR: Image samplers cannot be bound as an array; the values property cannot have a size above 1!");
			}
#endif // !NDEBUG

			// create uncreated sampler
			if (VK_NULL_HANDLE == _imageSamplers[i].values[0]->getVkSampler() ) {
				_imageSamplers[i].values[0]->create();
			}
		}
		//_imageSamplers.resize(_imageSamplersCreateInfos.size());
		/*const VkPhysicalDeviceProperties& properties = proc->_physicalDeviceProperties;
		for (int i = 0; i < _imageSamplersCreateInfos.size(); ++i) {
			float& ans = _imageSamplersCreateInfos[i].first.maxAnisotropy;
			ans = std::clamp(ans, 0.f, properties.limits.maxSamplerAnisotropy);
			if (vkCreateSampler(proc->_device, &_imageSamplersCreateInfos[i].first, nullptr, &_imageSamplers[i]) != VK_SUCCESS) {
				throw std::runtime_error("FAILED TO CREATE TEXTURE SAMPLER!");
			}
		}*/
		/* some shaders don't use image samplers
#ifndef NDEBUG
		else {
			printf("\nVAL: WARNING: shader::createImageSamplers was called, but createInfo for the image samplers has not been set!\n");
		}
#endif // !NDEBUG
		*/
	}

	const std::vector<char>& shader::getByteCode() noexcept {
		return _byteCode;
	}

	const fs::path& shader::getFilepath() noexcept {
		return _filepath;
	}

	VkShaderStageFlags shader::getStageFlags() noexcept {
		return _shaderStageFlags;
	}

	void shader::setStageFlags(const VkShaderStageFlags& stageFlags) {
		_shaderStageFlags  = stageFlags;
	}

	std::vector<VkDescriptorSetLayoutBinding>* shader::getLayoutBindings() noexcept {
		_layoutBindings.clear();

		uint32_t currentBindingIdx = 0;

		for (uint32_t i = 0; i < _UBO_Handles.size(); ++i) {
			VkDescriptorSetLayoutBinding uboLayoutBinding{};
			uboLayoutBinding.binding = _UBO_Handles[i].bindingIndex;
			uboLayoutBinding.descriptorCount = _UBO_Handles[i].values.size();
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.pImmutableSamplers = NULL;
			uboLayoutBinding.stageFlags = getStageFlags();
			_layoutBindings.push_back(uboLayoutBinding);
		}

		for (uint32_t i = 0; i < _SSBO_Handles.size(); ++i) {
			VkDescriptorSetLayoutBinding ssboLayoutBinding{};
			ssboLayoutBinding.binding = _SSBO_Handles[i].bindingIndex;
			ssboLayoutBinding.descriptorCount = _SSBO_Handles[i].values.size();
			ssboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			ssboLayoutBinding.pImmutableSamplers = NULL;
			ssboLayoutBinding.stageFlags = getStageFlags();
			_layoutBindings.push_back(ssboLayoutBinding);
		}


		std::vector<std::pair<val::sampler*, uint32_t>> combinedImageSamplers;
		std::vector< std::pair<val::sampler*, uint32_t>> standaloneImageSamplers;


		for (val::descriptorBinding<val::sampler*>& descBinding : _imageSamplers) {
#ifndef NDEBUG
			if (descBinding.values.size()>1) {
				printf("VAL: WARNING: descriptorBinding<val::sampler*> cannot have more than 1 value attached to it; it cannot be treated as an array!\n");
			}
#endif // NDEBUG
			for (val::sampler* sampler : descBinding.values) {
				if (sampler->getSamplerType() == combinedImage) {
					combinedImageSamplers.push_back({ sampler,descBinding.bindingIndex});
				}
				else if (sampler->getSamplerType() == standalone) {
					standaloneImageSamplers.push_back({ sampler,descBinding.bindingIndex });
				}
			}
		}

		for (uint32_t i = 0; i < combinedImageSamplers.size(); ++i) {
			VkDescriptorSetLayoutBinding samplerLayoutBinding{};
			samplerLayoutBinding.binding = combinedImageSamplers[i].second;
			samplerLayoutBinding.descriptorCount = 1;
			samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			samplerLayoutBinding.pImmutableSamplers = NULL;
			samplerLayoutBinding.stageFlags = getStageFlags();
			_layoutBindings.push_back(samplerLayoutBinding);
		}

		for (uint32_t i = 0; i < standaloneImageSamplers.size(); ++i) {
			VkDescriptorSetLayoutBinding samplerLayoutBinding{};
			samplerLayoutBinding.binding = standaloneImageSamplers[i].second;
			samplerLayoutBinding.descriptorCount = 1;
			samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
			samplerLayoutBinding.pImmutableSamplers = NULL;
			samplerLayoutBinding.stageFlags = getStageFlags();
			_layoutBindings.push_back(samplerLayoutBinding);
		}


		for (uint32_t i = 0; i < _textures.size(); ++i) {
			VkDescriptorSetLayoutBinding textureLayoutBinding{};
			textureLayoutBinding.binding = _textures[i].bindingIndex;
			textureLayoutBinding.descriptorCount = _textures[i].values.size();
			textureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			textureLayoutBinding.stageFlags = getStageFlags();
			textureLayoutBinding.pImmutableSamplers = NULL;
			_layoutBindings.push_back(textureLayoutBinding);
		}

		return &_layoutBindings;
	}
	
	std::vector<VkDescriptorSetLayoutBinding>* shader::getPushDescriptorLayoutBindings() noexcept {
		auto& pdlb_v = _pushDescriptorLayoutBindings;
		pdlb_v.resize(_pushDescriptors.size()); /*resize pd layouts to match the number of push descriptor*/

		for (uint_fast32_t i = 0; i < pdlb_v.size(); ++i) {
			const auto& pushDescriptor = _pushDescriptors[i];
			VkDescriptorSetLayoutBinding& textureLayoutBinding = pdlb_v[i];
			memset(&textureLayoutBinding, 0, sizeof(textureLayoutBinding)); /*0 init*/
			textureLayoutBinding.binding = pushDescriptor->getBindingIndex();
			textureLayoutBinding.descriptorCount = pushDescriptor->getValueCount();
			textureLayoutBinding.descriptorType = (VkDescriptorType)pushDescriptor->getDescType();
			textureLayoutBinding.stageFlags = getStageFlags();
		}
		return &pdlb_v;
	}


	// THE DESCRIPTOR WRITE IS INCOMPLETE, MUST BE FURTHER CONFIGURED IN VAL_PROC
	std::vector<VkWriteDescriptorSet>* shader::getDescriptorWrites() {
		_descriptorWrites.clear();

		uint32_t idx = 0;
		_descriptorWrites.resize(_descriptorWrites.size() + _UBO_Handles.size());
		for (size_t i = 0; i < _UBO_Handles.size(); ++i) {
			VkWriteDescriptorSet& descWrite = _descriptorWrites[i+idx];
				
			memset(&descWrite, 0, sizeof(VkWriteDescriptorSet)); // 0 clears mem
			// pack the value index into the puffer info, it will be used later to update descriptor sets
			descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descWrite.dstBinding = _UBO_Handles[i].bindingIndex;
			descWrite.descriptorType = _UBO_Handles[i].values[0]->getVkDescriptorType();
			descWrite.descriptorCount = _UBO_Handles[i].values.size();
			descWrite.pBufferInfo = VK_NULL_HANDLE;
		}
		idx = _descriptorWrites.size();

		_descriptorWrites.resize(_descriptorWrites.size() + _SSBO_Handles.size());
		for (size_t i = 0; i < _SSBO_Handles.size(); ++i) {
			VkWriteDescriptorSet& descWrite = _descriptorWrites[i+ idx];

			memset(&descWrite, 0, sizeof(VkWriteDescriptorSet)); // 0 clears mem
			// pack the value index into the puffer info, it will be used later to update descriptor sets
			descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descWrite.dstBinding = _SSBO_Handles[i].bindingIndex;
			descWrite.descriptorType = _SSBO_Handles[i].values[0]->getVkDescriptorType();
			descWrite.descriptorCount = _SSBO_Handles[i].values.size();
			descWrite.pBufferInfo = VK_NULL_HANDLE;
		}
		idx = _descriptorWrites.size();

		/*
#ifndef NDEBUG
		if ((_imageSamplers.size() != _imageViews.size())) {
			throw std::logic_error("FML: AN IMAGE_SAMPLER CANNOT WORK WITHOUT AN IMAGE_VIEW AND VICE VERSA");
			throw std::logic_error("FML: AN IMAGE_SAMPLER AND IMAGE_VIEWS MUST BE OF THE SAME SIZE");
		}
#endif // !NDEBUG
		*/

		if (_imageSamplers.size() > 0) {
			//// Image Sampler Descriptor Info
			std::vector<VkDescriptorImageInfo> imageInfos(_imageSamplers.size());

			for (int i = 0; i < imageInfos.size(); ++i) {
				_descriptorWrites.resize(_descriptorWrites.size() + 1);

				idx = _descriptorWrites.size() - 1;

				//// memset to VkWriteDescriptorSet 0
				memset(&_descriptorWrites[idx], 0, sizeof(VkWriteDescriptorSet));

				////////////////////////////////////////////////////////////////////
				imageInfos[i].sampler = _imageSamplers[i].values[0]->getVkSampler();
				imageInfos[i].imageView = *(_imageSamplers[i].values[0]->getImageView());
				imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				//// Set up the VkWriteDescriptorSet for Image Sampler
				_descriptorWrites[idx].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				_descriptorWrites[idx].dstBinding = 1; // Binding index for image sampler
				_descriptorWrites[idx].dstArrayElement = 0;
				if (_imageSamplers[i].values[0]->getSamplerType() == combinedImage) {
					_descriptorWrites[idx].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				}
				else if (_imageSamplers[i].values[0]->getSamplerType() == standalone) {
					_descriptorWrites[idx].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				}
				else {
					printf("VAL: ERROR: Samper at %h has an invalid sampler type of %d!\n", &_imageSamplers[i].values[0], _imageSamplers[i].values[0]->getSamplerType());
					throw std::runtime_error("INVALID SAMPLER TYPE");
				}
				_descriptorWrites[idx].descriptorCount = imageInfos.size();  // One image sampler descriptor
				if (imageInfos.size() > 0) {
					_descriptorWrites[idx].pImageInfo = imageInfos.data();
				}
				else {
					_descriptorWrites[idx].pImageInfo = VK_NULL_HANDLE;
				}
				_descriptorWrites[idx].pBufferInfo = VK_NULL_HANDLE;
				_descriptorWrites[idx].pTexelBufferView = VK_NULL_HANDLE;
			}
		}
		idx = _descriptorWrites.size();

		_descriptorWrites.resize(_descriptorWrites.size() + _textures.size());
		for (uint32_t i = 0; i < _textures.size(); ++i) {
			VkWriteDescriptorSet& descWrite = _descriptorWrites[i + idx];

			memset(&descWrite, 0, sizeof(VkWriteDescriptorSet)); // 0 clears mem
			// pack the value index into the puffer info, it will be used later to update descriptor sets
			descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descWrite.dstBinding = _textures[i].bindingIndex;
			descWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			descWrite.descriptorCount = _textures[i].values.size();
			descWrite.pImageInfo = NULL; // this is set later
		}
		idx = _descriptorWrites.size();

		return &_descriptorWrites;
	}

	std::vector<std::vector<std::vector<VkDescriptorBufferInfo>>>* shader::getDescriptorBufferInfos(VAL_PROC& proc) {
		auto& bufferWrites = _descriptorWriteBufferInfos;
		bufferWrites.clear();
		bufferWrites.resize(proc._MAX_FRAMES_IN_FLIGHT);
		for (uint8_t frame = 0; frame < proc._MAX_FRAMES_IN_FLIGHT; frame++) {
			for (size_t i = 0; i < _UBO_Handles.size(); ++i) {
				auto& uboHDL = _UBO_Handles[i];
				bufferWrites[frame].emplace_back();
				std::vector<VkDescriptorBufferInfo>& buffInfoArr = bufferWrites[frame].back();
				buffInfoArr.resize(uboHDL.values.size());
				for (size_t j = 0; j < buffInfoArr.size(); ++j) {
					VkDescriptorBufferInfo& buffInfo = buffInfoArr[j];
					buffInfo.buffer = uboHDL.values[j]->getBuffer(proc);
					buffInfo.range = uboHDL.values[j]->_size;
					buffInfo.offset = uboHDL.values[j]->_offset;
				}
			
			}

			for (size_t i = 0; i < _SSBO_Handles.size(); ++i) {
				auto& ssboHDL = _SSBO_Handles[i];
				bufferWrites[frame].emplace_back();
				std::vector<VkDescriptorBufferInfo>& buffInfoArr = bufferWrites[frame].back();
				buffInfoArr.resize(ssboHDL.values.size());
				for (size_t j = 0; j < buffInfoArr.size(); ++j) {
					VkDescriptorBufferInfo& buffInfo = buffInfoArr[j];
					memset(&buffInfo, 0, sizeof(VkDescriptorBufferInfo)); // 0 out memory
					buffInfo.buffer = ssboHDL.values[j]->getBuffers(proc)[frame];
					buffInfo.range = ssboHDL.values[j]->_size;
					buffInfo.offset = 0;
				}
			}
		}

		return &_descriptorWriteBufferInfos;
	}

	void shader::setVertexAttributes(const std::vector<VkVertexInputAttributeDescription>& attributes) {
		_attributes = attributes;
	}

	const std::vector<VkVertexInputAttributeDescription>& shader::getVertexAttributes() noexcept {
		return _attributes;
	}

	void shader::setBindingDescriptions(const std::vector<VkVertexInputBindingDescription>& bindingDescriptions) {
		_bindings = bindingDescriptions;
	}

	const std::vector<VkVertexInputBindingDescription>& shader::getBindingDescriptions() noexcept {
		return _bindings;
	}

	void shader::setPushConstant(pushConstantHandle* pushConstant) {
		_pushConstant = pushConstant;
		_pushConstant->_stageFlags |= _shaderStageFlags;
	}

	pushConstantHandle* shader::getPushConstant() noexcept {
		return _pushConstant;
	}

	void shader::addPushDescriptor(val::pushDescriptor& pushDesc) {
		_pushDescriptors.push_back(&pushDesc);
	}

	void shader::setPushDescriptors(const std::vector<val::pushDescriptor*> pushDescriptors) {
		_pushDescriptors = pushDescriptors;
	}

	void shader::setTextures(const std::vector<descriptorBinding<val::imageView*>> textures) {
		_textures = textures;
	}

	const std::vector<descriptorBinding<val::imageView*>> shader::getTextures() noexcept {
		return _textures;
	}

	void shader::setUBOs(const std::vector<descriptorBinding<UBO_Handle*>>& ubos) {
		_UBO_Handles = ubos;
	}

	const std::vector<descriptorBinding<val::UBO_Handle*>> shader::getUBOS() noexcept {
		return _UBO_Handles;
	}

	void shader::setSSBOs(const std::vector<descriptorBinding<SSBO_Handle*>>& SSBOs) {
		_SSBO_Handles = SSBOs;
	}

	const std::vector<descriptorBinding<val::SSBO_Handle*>> shader::getSSBOs() noexcept {
		return _SSBO_Handles;
	}

	void shader::updateImageSampler(VAL_PROC& proc, const pipelineCreateInfo& pipeline, std::pair<sampler&, uint32_t> sampler) {
		for (uint16_t i = 0; i < proc._MAX_FRAMES_IN_FLIGHT; ++i) {
			VkDescriptorSet& descriptorSet = proc._descriptorSets[pipeline.pipelineIdx][i];

			VkWriteDescriptorSet descriptorWrite;
			descriptorWrite.pNext = NULL;
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSet;
			descriptorWrite.dstBinding = sampler.second; // THIS MUST MATCH THE BINDING
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = (VkDescriptorType)sampler.first.getSamplerType();;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &sampler.first.getVkDescriptorImageInfo();

			vkUpdateDescriptorSets(proc._device, 1, &descriptorWrite, 0, nullptr);
		}
	}
	;
	void shader::updateImageSamplerAtFrame(VAL_PROC& proc, const pipelineCreateInfo& pipeline, std::pair<sampler&, uint32_t> sampler, const uint8_t frameInFlight)
	{
		VkWriteDescriptorSet descriptorWrite;
		descriptorWrite.pNext = NULL;
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = pipeline.getDescriptorSets(proc)[frameInFlight];
		descriptorWrite.dstBinding = sampler.second;
		descriptorWrite.descriptorType = (VkDescriptorType)sampler.first.getSamplerType();
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.pImageInfo = &sampler.first.getVkDescriptorImageInfo();
		vkUpdateDescriptorSets(proc._device, 1u, &descriptorWrite, 0, NULL);
	}

	void shader::updateTexture(VAL_PROC& proc, const pipelineCreateInfo& pipeline, std::pair<imageView&, uint32_t> texture, const uint16_t arrIdx)
	{

		for (uint16_t i = 0; i < proc._MAX_FRAMES_IN_FLIGHT; ++i) {
			const VkDescriptorSet& descriptorSet = proc._descriptorSets[pipeline.pipelineIdx][i];

			VkDescriptorImageInfo imgInfo = { NULL, texture.first.getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

			VkWriteDescriptorSet descriptorWrite;
			descriptorWrite.pNext = NULL;
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSet;
			descriptorWrite.dstBinding = texture.second; // THIS MUST MATCH THE BINDING
			descriptorWrite.dstArrayElement = arrIdx;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &imgInfo;
			vkUpdateDescriptorSets(proc._device, 1, &descriptorWrite, 0, nullptr);
		}
	}

	void shader::updateTextureAtFrame(VAL_PROC& proc, const pipelineCreateInfo& pipeline, std::pair<imageView&, uint32_t> texture, const uint8_t frameInFlight, const uint16_t arrIdx)
	{
			const VkDescriptorSet& descriptorSet = proc._descriptorSets[pipeline.pipelineIdx][frameInFlight];

			VkDescriptorImageInfo imgInfo = { NULL, texture.first.getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };

			VkWriteDescriptorSet descriptorWrite;
			descriptorWrite.pNext = NULL;
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSet;
			descriptorWrite.dstBinding = texture.second; // THIS MUST MATCH THE BINDING
			descriptorWrite.dstArrayElement = arrIdx;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pImageInfo = &imgInfo;
			vkUpdateDescriptorSets(proc._device, 1, &descriptorWrite, 0, nullptr);
	}


	void shader::updateUBO(VAL_PROC& proc, const pipelineCreateInfo& pipeline, std::pair<UBO_Handle&, uint32_t> UBO, const uint16_t arrIdx)
	{
		for (int_fast8_t frameIdx = 0; frameIdx < proc._MAX_FRAMES_IN_FLIGHT; ++frameIdx) {
			VkDescriptorBufferInfo buffInfo;
			buffInfo.buffer = UBO.first.getBuffer(proc);
			buffInfo.range = UBO.first._size;
			buffInfo.offset = UBO.first._offset;

			const VkDescriptorSet& descriptorSet = proc._descriptorSets[pipeline.pipelineIdx][frameIdx];

			VkWriteDescriptorSet descriptorWrite;
			descriptorWrite.pNext = NULL;
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSet;
			descriptorWrite.dstBinding = UBO.second; // THIS MUST MATCH THE BINDING
			descriptorWrite.dstArrayElement = arrIdx;
			descriptorWrite.descriptorType = UBO.first.getVkDescriptorType();
			descriptorWrite.descriptorCount = 1;
			vkUpdateDescriptorSets(proc._device, 1, &descriptorWrite, 0, nullptr);
		}
	}

	void shader::updateUBOatFrame(VAL_PROC& proc, const pipelineCreateInfo& pipeline, std::pair<UBO_Handle&, uint32_t> UBO, const uint8_t frameInFlight, const uint16_t arrIdx)
	{
		VkDescriptorBufferInfo buffInfo;
		buffInfo.buffer = UBO.first.getBuffer(proc);
		buffInfo.range = UBO.first._size;
		buffInfo.offset = UBO.first._offset;

		const VkDescriptorSet& descriptorSet = proc._descriptorSets[pipeline.pipelineIdx][frameInFlight];

		VkWriteDescriptorSet descriptorWrite;
		descriptorWrite.pNext = NULL;
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSet;
		descriptorWrite.dstBinding = UBO.second; // THIS MUST MATCH THE BINDING
		descriptorWrite.dstArrayElement = arrIdx;
		descriptorWrite.descriptorType = UBO.first.getVkDescriptorType();
		descriptorWrite.descriptorCount = 1;
		vkUpdateDescriptorSets(proc._device, 1, &descriptorWrite, 0, nullptr);
	}

	void shader::updateSSBO(VAL_PROC& proc, const pipelineCreateInfo& pipeline, std::pair<SSBO_Handle, uint32_t> SSBO, const uint16_t arrIdx)
	{
		for (int_fast8_t frameIdx = 0; frameIdx < proc._MAX_FRAMES_IN_FLIGHT; ++frameIdx) {
			VkDescriptorBufferInfo buffInfo;
			buffInfo.buffer = SSBO.first.getBuffers(proc)[frameIdx];
			buffInfo.range = SSBO.first._size;
			buffInfo.offset = 0;

			const VkDescriptorSet& descriptorSet = proc._descriptorSets[pipeline.pipelineIdx][frameIdx];

			VkWriteDescriptorSet descriptorWrite;
			descriptorWrite.pNext = NULL;
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSet;
			descriptorWrite.dstBinding = SSBO.second; // THIS MUST MATCH THE BINDING
			descriptorWrite.dstArrayElement = arrIdx;
			descriptorWrite.descriptorType = SSBO.first.getVkDescriptorType();
			descriptorWrite.descriptorCount = 1;
			vkUpdateDescriptorSets(proc._device, 1, &descriptorWrite, 0, nullptr);
		}
	}

	void shader::updateSSBOatFrame(VAL_PROC& proc, const pipelineCreateInfo& pipeline, std::pair<SSBO_Handle, uint32_t> SSBO, const uint8_t frameInFlight, const uint16_t arrIdx)
	{
		VkDescriptorBufferInfo buffInfo;
		buffInfo.buffer = SSBO.first.getBuffers(proc)[frameInFlight];
		buffInfo.range = SSBO.first._size;
		buffInfo.offset = 0;

		const VkDescriptorSet& descriptorSet = proc._descriptorSets[pipeline.pipelineIdx][frameInFlight];

		VkWriteDescriptorSet descriptorWrite;
		descriptorWrite.pNext = NULL;
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSet;
		descriptorWrite.dstBinding = SSBO.second; // THIS MUST MATCH THE BINDING
		descriptorWrite.dstArrayElement = arrIdx;
		descriptorWrite.descriptorType = SSBO.first.getVkDescriptorType();
		descriptorWrite.descriptorCount = 1;
		vkUpdateDescriptorSets(proc._device, 1, &descriptorWrite, 0, nullptr);
	}
}