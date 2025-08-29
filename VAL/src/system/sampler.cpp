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
#include <unordered_map>

namespace val 
{

	std::unordered_map<Sampler*, VkSamplerCreateInfo>* createInfos;
	
	struct tmp_struct {
		tmp_struct() {
			createInfos = new std::unordered_map<Sampler*, VkSamplerCreateInfo>;
		}
		~tmp_struct() {
			valid = false;
			delete createInfos;
		}

		bool valid = false;
	};

	tmp_struct& getTmp() {
		static tmp_struct instance;
		return instance;
	}

	void checkCreateInfoValidity() {
		auto& tmp = getTmp();
		if (!tmp.valid) {
			createInfos = new std::unordered_map<Sampler*, VkSamplerCreateInfo>;
			tmp.valid = true;
		}
	}

	VAL_RETURN_CODE Sampler::create(VkDevice device, bool keepCreateInfo)
	{
		checkCreateInfoValidity();

#ifndef NDEBUG
		if (_imgView == NULL && _samplerType != SAMPLER_TYPE::standalone)
		{
			dbg::printError("Sampler::create: standalone Sampler @ %p cannot be created without an image view.",this);
				return VAL_FAILURE;
		}
#endif // !NDEBUG

		VkSampler& sampler = _VKdescriptorInfo.sampler;

#ifndef NDEBUG
		if (sampler != VK_NULL_HANDLE)
		{
			dbg::printError("Sampler::create: This function should never be called on a sampler which has already been created.", this);
			throw std::runtime_error("Sampler::create: This function should never be called on a sampler which has already been created.");
		}
#endif // !NDEBUG

		VkSamplerCreateInfo& createInfo = (*createInfos)[this];
		
		if (vkCreateSampler(device, &createInfo, nullptr, &sampler) != VK_SUCCESS) {
			dbg::printError("Sampler::create: Failed to create VkSampler of Sampler %p", this);
#ifndef NDEBUG
			throw std::runtime_error("Failed to create VkSampler");
#endif // !
			return VAL_FAILURE;
		}
		dbg::recordVkObjectCreation(device, sampler);

		if (keepCreateInfo == false) {
			(*createInfos).erase(this);
		}
		return VAL_SUCCESS;
	}

	void Sampler::destroy(VkDevice device) {
		VkSampler& sampler = _VKdescriptorInfo.sampler;
		if (sampler) {
			vkDestroySampler(device, sampler, NULL);
			dbg::recordVkObjectDestruction(device, sampler);
			sampler = NULL;
		}
	}


	void Sampler::setSamplerType(const SAMPLER_TYPE& type)
	{
		_samplerType = type;
	}

	// An anisoLevel greater than 0 will enable anisotropic filtering, if it's equal to 0 it will be disabled.
	void Sampler::setMaxAnisotropy(const float& anisoLevel)
	{
		VkSamplerCreateInfo& createInfo = (*createInfos)[this];
		if (anisoLevel > 0) {
			createInfo.anisotropyEnable = VK_TRUE;
			createInfo.maxAnisotropy = anisoLevel;
		}
		else {
			createInfo.anisotropyEnable = VK_FALSE;
			createInfo.maxAnisotropy = 0.f;
		}
	}

	void Sampler::setMagnificationFilter(const VkFilter& filterType)
	{
		VkSamplerCreateInfo& createInfo = (*createInfos)[this];
		createInfo.magFilter = filterType;
	}

	void Sampler::setMinificationFilter(const VkFilter& filterType)
	{
		VkSamplerCreateInfo& createInfo = (*createInfos)[this];
		createInfo.minFilter = filterType;
	}

	void Sampler::setMipmapMode(VkSamplerMipmapMode mipMapMode)
	{
		VkSamplerCreateInfo& createInfo = (*createInfos)[this];
		createInfo.mipmapMode = mipMapMode;
	}

	// U direction (horizontal)
	void Sampler::setAddressModeU(const VkSamplerAddressMode& addrMode)
	{
		VkSamplerCreateInfo& createInfo = (*createInfos)[this];
		createInfo.addressModeU = addrMode;
	}
	// V direction (vertical)
	void Sampler::setAddressModeV(const VkSamplerAddressMode& addrMode)
	{
		VkSamplerCreateInfo& createInfo = (*createInfos)[this];
		createInfo.addressModeV = addrMode;
	}
	// W direction (depth)
	void Sampler::setAddressModeW(const VkSamplerAddressMode& addrMode)
	{
		VkSamplerCreateInfo& createInfo = (*createInfos)[this];
		createInfo.addressModeW = addrMode;
	}

	void Sampler::useUnnormalizedCoordinates(const bool& val)
	{
		VkSamplerCreateInfo& createInfo = (*createInfos)[this];
		createInfo.unnormalizedCoordinates = val;

	}

	void Sampler::setCompareMode(VkCompareOp cmpOp/*Set to VK_COMPARE_OP_NEVER to disable*/)
	{
		VkSamplerCreateInfo& createInfo = (*createInfos)[this];
		switch (cmpOp)
		{
		case VK_COMPARE_OP_NEVER:
			createInfo.compareEnable = VK_FALSE;
			createInfo.compareOp = cmpOp;
			break;
		case VK_COMPARE_OP_MAX_ENUM:
			createInfo.compareEnable = VK_FALSE;
			createInfo.compareOp = cmpOp;
			break;
		default:
			createInfo.compareEnable = VK_TRUE;
			createInfo.compareOp = cmpOp;
			break;
		}
	}

	void Sampler::setMipLodBias(const float& mipLOD)
	{
		VkSamplerCreateInfo& createInfo = (*createInfos)[this];
		createInfo.mipLodBias = mipLOD;
	}

	void Sampler::setFromVkSamplerCreateInfo(const VkSamplerCreateInfo& createInfo)
	{
		VkSamplerCreateInfo& createInfo__ = (*createInfos)[this];
		createInfo__ = createInfo;
	}

	void Sampler::setBorderColor(const VkBorderColor color) {
		VkSamplerCreateInfo& createInfo = (*createInfos)[this];
		createInfo.borderColor = color;
	}

	VkBorderColor Sampler::getVkBorderColor() const{
		const VkSamplerCreateInfo& createInfo = (*createInfos)[(Sampler*)(this)];
		return createInfo.borderColor;
	}

	std::optional<VkSamplerCreateInfo> Sampler::getCreateInfo() 
	{
		if ((*createInfos).count(this) == 0) {
			return std::nullopt;
		}
		return { (*createInfos)[this] };
	}

	void Sampler::initDefaultCreateInfoValues()
	{
		checkCreateInfoValidity();

		VkSamplerCreateInfo& createInfo = (*createInfos)[this];
		createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.pNext = NULL;
		createInfo.anisotropyEnable = VK_FALSE;  // Enable anisotropic filtering
		createInfo.maxAnisotropy = 0.0f;
		createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.magFilter = VK_FILTER_LINEAR;
		createInfo.minFilter = VK_FILTER_LINEAR;
		createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		createInfo.compareEnable = VK_FALSE;
		createInfo.compareOp = VK_COMPARE_OP_NEVER;
		createInfo.unnormalizedCoordinates = VK_FALSE;  // Use normalized coordinates
		createInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
	}

	/////////////////////////////////////////

	void Sampler::bindImageView(ImageView& imageView)
	{
		_imgView = &imageView;
		_VKdescriptorInfo.imageView = _imgView->getImageView();
	}


	ImageView* Sampler::getImageView()
	{
		return _imgView;
	}

	const SAMPLER_TYPE& Sampler::getSamplerType() const {
		return _samplerType;
	}

	const VkSampler& Sampler::getVkSampler() const
	{
		return _VKdescriptorInfo.sampler;
	}

	VkDescriptorImageInfo& Sampler::getVkDescriptorImageInfo() 
	{
		VkSampler& sampler = _VKdescriptorInfo.sampler;
		_VKdescriptorInfo.imageView = _imgView->getImageView();
		_VKdescriptorInfo.sampler = sampler;
		_VKdescriptorInfo.imageLayout = _imgView->getLayout();
		return _VKdescriptorInfo;
	}

	/************************************************************************************************/
}