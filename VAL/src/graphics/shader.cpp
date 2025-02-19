#include <VAL/lib/graphics/shader.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	bool shader::loadFromFile(fs::path filepath) {
		_filepath = fs::absolute(filepath);

		if (readByteFile(_filepath.string(), &_byteCode)) {
			return true;
		}
#ifndef  NDEBUG
		else {
			printf("\nVAL: FAILED TO READ FILE FROM DISK: %s\n", filepath.c_str());
		}
#endif // ! NDEBUG
	}

	void shader::setEntryPoint(const std::string& entryPoint) {
		_entryPoint = entryPoint;
	}

	const std::string& shader::getEntryPoint() {
		return _entryPoint;
	}

	void shader::setUniformBuffers(const std::vector<UBO_Handle*> UBOs) {
		_UBO_Handles = UBOs;
	}

	/*void shader::setUniformBufferData(void* UBO, const size_t& sizeOfUBO, const uint32_t UBO_Count) {
		_UBO = UBO;
		_sizeOfUBO = sizeOfUBO;
		_UBO_Count = UBO_Count;
	}*/

	void shader::setImageSamplers(const std::vector<VkSamplerCreateInfo> samplerInfo) {
		_imageSamplersCreateInfos = samplerInfo;
	}

	void shader::createImageSamplers(VAL_PROC* procFML) {
		if (_imageSamplersCreateInfos.has_value()) {
			_imageSamplers.resize(_imageSamplersCreateInfos.value().size());

			const VkPhysicalDeviceProperties& properties = procFML->_physicalDeviceProperties;

			for (int i = 0; i < _imageSamplersCreateInfos.value().size(); ++i) {
				float& ans = _imageSamplersCreateInfos.value()[i].maxAnisotropy;
				ans = std::clamp(ans, 0.f, properties.limits.maxSamplerAnisotropy);
				if (vkCreateSampler(procFML->_device, &_imageSamplersCreateInfos.value()[i], nullptr, &_imageSamplers[i]) != VK_SUCCESS) {
					throw std::runtime_error("FAILED TO CREATE TEXTURE SAMPLER!");
				}
			}
		}
		/* some shaders don't use image samplers
#ifndef NDEBUG
		else {
			printf("\nVAL: WARNING: shader::createImageSamplers was called, but createInfo for the image samplers has not been set!\n");
		}
#endif // !NDEBUG
		*/
	}

	void shader::setImageView(const std::vector<VkImageView*> imageView) {
		_imageViews = imageView;
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

		if (_UBO_Handles.size() > 0) {
			VkDescriptorSetLayoutBinding uboLayoutBinding{};
			uboLayoutBinding.binding = currentBindingIdx; currentBindingIdx++;
			uboLayoutBinding.descriptorCount = _UBO_Handles.size();
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.pImmutableSamplers = nullptr;
			uboLayoutBinding.stageFlags = getStageFlags();
			_layoutBindings.push_back(uboLayoutBinding);
		}

		if (_SSBO_Handles.size() > 0) {
			VkDescriptorSetLayoutBinding ssboLayoutBinding{};
			ssboLayoutBinding.binding = currentBindingIdx; currentBindingIdx++;
			ssboLayoutBinding.descriptorCount = _SSBO_Handles.size();
			ssboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			ssboLayoutBinding.pImmutableSamplers = nullptr;
			ssboLayoutBinding.stageFlags = getStageFlags();
			_layoutBindings.push_back(ssboLayoutBinding);
		}

		if (_imageSamplersCreateInfos.has_value()) {
			if (_imageSamplersCreateInfos.value().size() > 0) {
				VkDescriptorSetLayoutBinding samplerLayoutBinding{};
				samplerLayoutBinding.binding = currentBindingIdx; currentBindingIdx++;
				samplerLayoutBinding.descriptorCount = 1;
				samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				samplerLayoutBinding.pImmutableSamplers = nullptr;
				samplerLayoutBinding.stageFlags = getStageFlags();
				_layoutBindings.push_back(samplerLayoutBinding);
			}
		}
		return &_layoutBindings;
	}

	// THE DESCRIPTOR WRITE IS INCOMPLETE, MUST BE FURTHER CONFIGURED IN FML_PROC
	std::vector<VkWriteDescriptorSet>* val::shader::getDescriptorWrites() noexcept {
		_descriptorWrites.clear();

		uint32_t idx = 0;
		if (_UBO_Handles.size() > 0) {
			_descriptorWrites.resize(_descriptorWrites.size() + _UBO_Handles.size());
			//idx = _descriptorWrites.size() - 1;
			for (size_t i = 0; i < _UBO_Handles.size(); ++i) {
				VkWriteDescriptorSet& descWrite = _descriptorWrites[i];
				
				memset(&descWrite, 0, sizeof(VkWriteDescriptorSet)); // 0 clears mem

				descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descWrite.dstBinding = i;
				descWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descWrite.descriptorCount = 1;
				descWrite.pBufferInfo = VK_NULL_HANDLE;
			}
			// memset to VkWriteDescriptorSet 0
			//memset(&_descriptorWrites[0], 0, sizeof(VkWriteDescriptorSet)); // 0 clears mem


			/*_descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			_descriptorWrites[0].dstBinding = 0;
			_descriptorWrites[0].dstArrayElement = 0;
			_descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			_descriptorWrites[0].descriptorCount = _UBOs.size();
			_descriptorWrites[0].pTexelBufferView = VK_NULL_HANDLE;
			_descriptorWrites[0].pImageInfo = VK_NULL_HANDLE;
			_descriptorWrites[0].pBufferInfo = VK_NULL_HANDLE;*/
		}

#ifndef NDEBUG
		if ((_imageSamplers.size() != _imageViews.size())) {
			throw std::logic_error("FML: AN IMAGE_SAMPLER CANNOT WORK WITHOUT AN IMAGE_VIEW AND VICE VERSA");
			throw std::logic_error("FML: AN IMAGE_SAMPLER AND IMAGE_VIEWS MUST BE OF THE SAME SIZE");
		}
#endif // !NDEBUG

		if (_imageSamplers.size() > 0 && _imageViews.size() > 0) {
			_descriptorWrites.resize(_descriptorWrites.size() + 1);

			idx = _descriptorWrites.size() - 1;

			//// memset to VkWriteDescriptorSet 0
			memset(&_descriptorWrites[idx], 0, sizeof(VkWriteDescriptorSet));

			//// Image Sampler Descriptor Info
			std::vector<VkDescriptorImageInfo> imageInfos(_imageSamplers.size());

			// this could be optimized by directly loading it to the descriptor writes
			for (int i = 0; i < imageInfos.size(); ++i) {
				imageInfos[i].sampler = _imageSamplers[i];
				imageInfos[i].imageView = *_imageViews[i];
				imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			}

			//// Set up the VkWriteDescriptorSet for Image Sampler
			_descriptorWrites[idx].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			_descriptorWrites[idx].dstBinding = 1; // Binding index for image sampler
			_descriptorWrites[idx].dstArrayElement = 0;
			_descriptorWrites[idx].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
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

		return &_descriptorWrites;
	}

	void shader::setVertexAttributes(VkVertexInputAttributeDescription* attributes, const uint32_t& attribCount) {
		_attributes = attributes;
		_attribCount = attribCount;
	}

	VkVertexInputAttributeDescription* shader::getVertexAttributes() noexcept {
		return _attributes;
	}

	void shader::setBindingDescription(const VkVertexInputBindingDescription& bindingDescription) {
		_bindingDescription = bindingDescription;
	}

	const VkVertexInputBindingDescription& shader::getBindingDescription() noexcept {
		return _bindingDescription;
	}

	const uint32_t& shader::getVertexAttributesCount() noexcept {
		return _attribCount;
	}

	void shader::setPushConstants(const std::vector<pushConstantHandle*>& pushConstants) {
		_pushConstants = pushConstants;
		for (auto& PC_Hdl : _pushConstants) {
			PC_Hdl->_stageFlags = VkShaderStageFlagBits(PC_Hdl->_stageFlags | _shaderStageFlags);
		}
	}

	const std::vector<pushConstantHandle*>& shader::getPushConstants() noexcept {
		return _pushConstants;
	}
}