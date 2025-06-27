/*
Copyright © 2025 Tripp Robins

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the “Software”), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <VAL/lib/system/sampler.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/graphicsPipeline.hpp>

namespace val {

	void Sampler::create() 
	{
		const VkPhysicalDeviceProperties& properties = _proc._physicalDeviceProperties;
		
		float& ans = _samplerCreateInfo.maxAnisotropy;
		ans = std::clamp(ans, 0.f, properties.limits.maxSamplerAnisotropy);
		if (vkCreateSampler(_proc._device, &_samplerCreateInfo, nullptr, &_sampler) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO CREATE TEXTURE SAMPLER!");
		}
		_VKdescriptorInfo.sampler = _sampler;
	}

	void Sampler::destroy() {
		if (_sampler) {
			vkDestroySampler(_proc._device, _sampler, NULL);
			_sampler = NULL;
		}
	}

	void Sampler::bindImageView(ImageView& imageView) {
		_imgView = &imageView;
		_VKdescriptorInfo.imageView = _imgView->getImageView();
		_VKdescriptorInfo.sampler = _sampler;
	}

	ImageView* Sampler::getImageView() {
		return _imgView;
	}
	
	void Sampler::setSamplerType(const samplerType& type) {
		_samplerType = type;
	}


	void Sampler::setMaxAnisotropy(const float& anisoLevel) {
		if (anisoLevel > 0) {
			_samplerCreateInfo.anisotropyEnable = VK_TRUE;
			_samplerCreateInfo.maxAnisotropy = anisoLevel;
		} else{
			_samplerCreateInfo.anisotropyEnable = VK_FALSE;
			_samplerCreateInfo.maxAnisotropy = 0.f;
		}
	}

	void Sampler::setMagnificationFilter(const VkFilter& filterType) {
		_samplerCreateInfo.magFilter = filterType;
	}

	void Sampler::setMinificationFilter(const VkFilter& filterType) {
		_samplerCreateInfo.minFilter = filterType;
	}

	void Sampler::setMipmapMode(VkSamplerMipmapMode mipMapMode) {
		_samplerCreateInfo.mipmapMode = mipMapMode;
	}

	// U direction (horizontal)
	void Sampler::setAddressModeU(const VkSamplerAddressMode& addrMode) {
		_samplerCreateInfo.addressModeU = addrMode;
	}
	// V direction (vertical)
	void Sampler::setAddressModeV(const VkSamplerAddressMode& addrMode) {
		_samplerCreateInfo.addressModeV = addrMode;
	}
	// W direction (depth)
	void Sampler::setAddressModeW(const VkSamplerAddressMode& addrMode) {
		_samplerCreateInfo.addressModeW = addrMode;
	}

	void Sampler::useNormalizedCoordinates(const bool& val) {
		_samplerCreateInfo.unnormalizedCoordinates = !val;
	}

	void Sampler::setCompareMode(VkCompareOp cmpOp/*Set to VK_COMPARE_OP_NEVER to disable*/) {
		switch (cmpOp)
		{
		case VK_COMPARE_OP_NEVER:
			_samplerCreateInfo.compareEnable = VK_FALSE;
			_samplerCreateInfo.compareOp = cmpOp;
			break;
		default:
			_samplerCreateInfo.compareEnable = VK_TRUE;
			_samplerCreateInfo.compareOp = cmpOp;
			break;
		}
	}

	void Sampler::setMipLodBias(const float& mipLOD) {
		_samplerCreateInfo.mipLodBias = mipLOD;
	}

	void Sampler::setFromVkSamplerCreateInfo(const VkSamplerCreateInfo& createInfo) {
		_samplerCreateInfo = createInfo;
	}

	/////////////////////////////////////////

	const samplerType& Sampler::getSamplerType() {
		return _samplerType;
	}


	const float& Sampler::getMaxAnisotropy() {
		return _samplerCreateInfo.maxAnisotropy;
	}
	const VkFilter& Sampler::getMagnificationFilter() {
		return _samplerCreateInfo.magFilter;
	}
	const VkFilter& Sampler::getMinificationFilter() {
		return _samplerCreateInfo.minFilter;
	}
	const VkSamplerMipmapMode& Sampler::getMipmapMode() {
		return _samplerCreateInfo.mipmapMode;
	}
	const VkSamplerAddressMode& Sampler::getAddressModeU() {
		return _samplerCreateInfo.addressModeU;
	}
	const VkSamplerAddressMode& Sampler::getAddressModeV() {
		return _samplerCreateInfo.addressModeV;
	}
	const VkSamplerAddressMode& Sampler::getAddressModeW() {
		return _samplerCreateInfo.addressModeW;
	}
	const bool& Sampler::unnormalizedCoordinates() {
		return _samplerCreateInfo.unnormalizedCoordinates;
	}
	const VkCompareOp& Sampler::getCompareMode() {
		return _samplerCreateInfo.compareOp;
	}
	const VkBool32& Sampler::getCompareEnabled() {
		return _samplerCreateInfo.compareEnable;
	}

	const VkSamplerCreateInfo& Sampler::getSamplerCreateInfo() {
		return _samplerCreateInfo;
	}

	VkSampler& Sampler::getVkSampler() {
		return _sampler;
	}

	VkDescriptorImageInfo& Sampler::getVkDescriptorImageInfo() {
		_VKdescriptorInfo.imageView = _imgView->getImageView();
		_VKdescriptorInfo.sampler = _sampler;
		_VKdescriptorInfo.imageLayout = _imgView->getLayout();
		return _VKdescriptorInfo;
	}

	/************************************************************************************************/
}