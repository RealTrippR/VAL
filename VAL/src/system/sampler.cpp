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
#include <VAL/lib/system/graphicsPipelineCreateInfo.hpp>

namespace val {

	void sampler::create() {
		const VkPhysicalDeviceProperties& properties = _proc._physicalDeviceProperties;
		
		float& ans = _samplerCreateInfo.maxAnisotropy;
		ans = std::clamp(ans, 0.f, properties.limits.maxSamplerAnisotropy);
		if (vkCreateSampler(_proc._device, &_samplerCreateInfo, nullptr, &_sampler) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO CREATE TEXTURE SAMPLER!");
		}
		_VKdescriptorInfo.sampler = _sampler;
	}

	void sampler::destroy() {
		if (_sampler) {
			vkDestroySampler(_proc._device, _sampler, NULL);
			_sampler = NULL;
		}
	}

	void sampler::bindImageView(imageView& imageView) {
		_imgView = &imageView;
		_VKdescriptorInfo.imageView = _imgView->getImageView();
		_VKdescriptorInfo.sampler = _sampler;
	}

	imageView* sampler::getImageView() {
		return _imgView;
	}
	
	void sampler::setSamplerType(const samplerType& type) {
		_samplerType = type;
	}


	void sampler::setMaxAnisotropy(const float& anisoLevel) {
		if (anisoLevel > 0) {
			_samplerCreateInfo.anisotropyEnable = VK_TRUE;
			_samplerCreateInfo.maxAnisotropy = anisoLevel;
		} else{
			_samplerCreateInfo.anisotropyEnable = VK_FALSE;
			_samplerCreateInfo.maxAnisotropy = 0.f;
		}
	}

	void sampler::setMagnificationFilter(const VkFilter& filterType) {
		_samplerCreateInfo.magFilter = filterType;
	}

	void sampler::setMinificationFilter(const VkFilter& filterType) {
		_samplerCreateInfo.minFilter = filterType;
	}

	void sampler::setMipmapMode(VkSamplerMipmapMode mipMapMode) {
		_samplerCreateInfo.mipmapMode = mipMapMode;
	}

	// U direction (horizontal)
	void sampler::setAddressModeU(const VkSamplerAddressMode& addrMode) {
		_samplerCreateInfo.addressModeU = addrMode;
	}
	// V direction (vertical)
	void sampler::setAddressModeV(const VkSamplerAddressMode& addrMode) {
		_samplerCreateInfo.addressModeV = addrMode;
	}
	// W direction (depth)
	void sampler::setAddressModeW(const VkSamplerAddressMode& addrMode) {
		_samplerCreateInfo.addressModeW = addrMode;
	}

	void sampler::useNormalizedCoordinates(const bool& val) {
		_samplerCreateInfo.unnormalizedCoordinates = !val;
	}

	void sampler::setCompareMode(VkCompareOp cmpOp/*Set to VK_COMPARE_OP_NEVER to disable*/) {
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

	void sampler::setMipLodBias(const float& mipLOD) {
		_samplerCreateInfo.mipLodBias = mipLOD;
	}

	void sampler::setFromVkSamplerCreateInfo(const VkSamplerCreateInfo& createInfo) {
		_samplerCreateInfo = createInfo;
	}

	/////////////////////////////////////////

	const samplerType& sampler::getSamplerType() {
		return _samplerType;
	}


	const float& sampler::getMaxAnisotropy() {
		return _samplerCreateInfo.maxAnisotropy;
	}
	const VkFilter& sampler::getMagnificationFilter() {
		return _samplerCreateInfo.magFilter;
	}
	const VkFilter& sampler::getMinificationFilter() {
		return _samplerCreateInfo.minFilter;
	}
	const VkSamplerMipmapMode& sampler::getMipmapMode() {
		return _samplerCreateInfo.mipmapMode;
	}
	const VkSamplerAddressMode& sampler::getAddressModeU() {
		return _samplerCreateInfo.addressModeU;
	}
	const VkSamplerAddressMode& sampler::getAddressModeV() {
		return _samplerCreateInfo.addressModeV;
	}
	const VkSamplerAddressMode& sampler::getAddressModeW() {
		return _samplerCreateInfo.addressModeW;
	}
	const bool& sampler::normalizedCoordinates() {
		return !_samplerCreateInfo.unnormalizedCoordinates;
	}
	const VkCompareOp& sampler::getCompareMode() {
		return _samplerCreateInfo.compareOp;
	}
	const VkBool32& sampler::getCompareEnabled() {
		return _samplerCreateInfo.compareEnable;
	}

	const VkSamplerCreateInfo& sampler::getSamplerCreateInfo() {
		return _samplerCreateInfo;
	}

	VkSampler& sampler::getVkSampler() {
		return _sampler;
	}

	VkDescriptorImageInfo& sampler::getVkDescriptorImageInfo() {
		_VKdescriptorInfo.imageView = _imgView->getImageView();
		_VKdescriptorInfo.sampler = _sampler;
		_VKdescriptorInfo.imageLayout = _imgView->getLayout();
		return _VKdescriptorInfo;
	}

	/************************************************************************************************/
}