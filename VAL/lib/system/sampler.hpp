#ifndef VAL_SAMPLER_HPP
#define VAL_SAMPLER_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/imageView.hpp>

namespace val {
	enum samplerType {
		combinedImage, // equivalent to VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER 
		standalone, // equivalent to VK_DESCRIPTOR_TYPE_SAMPLER 
		immutable //  // equivalent to combinedImage except it cannot be changed and is baked into the graphics pipeline
	};

	class sampler {
	public:
		sampler(VAL_PROC& proc) : _proc(proc) { initDefaultValues(); };
		sampler(VAL_PROC& proc, samplerType samplerType = combinedImage) : _proc(proc), _samplerType(samplerType) { initDefaultValues(); };
		sampler(VAL_PROC& proc, val::imageView* imgView, samplerType samplerType = combinedImage) : _proc(proc), _imgView(imgView), _samplerType(samplerType) { initDefaultValues(); };
	public:
		void create();

		//void recreate();
	public:
		void bindImageView(imageView& imageView);

		imageView* getImageView();
	public:
		void setSamplerType(const samplerType& type);

		void setMaxAnisotropy(const float& anisoLevel);
		
		void setMagnificationFilter(const VkFilter& filterType);
		
		void setMinificationFilter(const VkFilter& filterType);

		void setMipmapMode(VkSamplerMipmapMode mipMapMode);

		// U direction (horizontal)
		void setAddressModeU(const VkSamplerAddressMode& addrMode);
		// V direction (vertical)
		void setAddressModeV(const VkSamplerAddressMode& addrMode);
		// W direction (depth)
		void setAddressModeW(const VkSamplerAddressMode& addrMode);

		void useNormalizedCoordinates(const bool& val);

		void setCompareMode(VkCompareOp cmpOp/*Set to VK_COMPARE_OP_NEVER to disable*/);

		void setMipLodBias(const float& mipLOD);

		void setFromVkSamplerCreateInfo(const VkSamplerCreateInfo& createInfo);

	public:

		const samplerType& getSamplerType();

		const float& getMaxAnisotropy();

		const VkFilter& getMagnificationFilter();

		const VkFilter& getMinificationFilter();

		const VkSamplerMipmapMode& getMipmapMode();

		const VkSamplerAddressMode& getAddressModeU();

		const VkSamplerAddressMode& getAddressModeV();

		const VkSamplerAddressMode& getAddressModeW();

		const bool& normalizedCoordinates();

		const VkCompareOp& getCompareMode();

		const VkBool32& getCompareEnabled();

		const VkSamplerCreateInfo& getSamplerCreateInfo();

		VkSampler& getVkSampler();

	private:
		inline void initDefaultValues() {
			_samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			_samplerCreateInfo.anisotropyEnable = VK_FALSE;  // Enable anisotropic filtering
			_samplerCreateInfo.maxAnisotropy = 16.0f;
			_samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			_samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			_samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			_samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
			_samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
			_samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			_samplerCreateInfo.compareEnable = VK_FALSE; 
			_samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
			_samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;  // Use normalized coordinates
			_samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		}

	protected:
		friend VAL_PROC;
		VAL_PROC& _proc;
		samplerType _samplerType = combinedImage;
		VkSampler _sampler = VK_NULL_HANDLE;
		VkSamplerCreateInfo _samplerCreateInfo{};
		imageView* _imgView;
	};
}

#endif // !VAL_SAMPLER_HPP
