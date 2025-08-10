#ifndef VAL_SAMPLER_HPP
#define VAL_SAMPLER_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/imageView.hpp>

namespace val {
	enum SAMPLER_TYPE {
		combinedImage = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, // equivalent to VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER 
		standalone = VK_DESCRIPTOR_TYPE_SAMPLER, // equivalent to VK_DESCRIPTOR_TYPE_SAMPLER 
		immutable = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER //  // equivalent to combinedImage except it cannot be changed and is baked into the graphics pipeline
	};

	class Sampler {
	public:

		Sampler()
		{ 
			initDefaultCreateInfoValues();
		};

		Sampler(SAMPLER_TYPE samplerType) : _samplerType(samplerType) 
		{
			initDefaultCreateInfoValues(); 
		};
		Sampler(val::ImageView& imgView, SAMPLER_TYPE samplerType = combinedImage, VkCompareOp compareop = VK_COMPARE_OP_NEVER) : _samplerType(samplerType) 
		{
			initDefaultCreateInfoValues();
			setCompareMode(compareop);
			bindImageView(imgView); 
		};

		inline VkDescriptorType getVkDescriptorType() {
			return (VkDescriptorType)_samplerType;
		}

		inline static void toObjectDescriptorInfo(ObjectDescriptorInfo* descInfo)
		{
			Sampler* self = (Sampler*)descInfo->valObject;
			if (self->getImageView()) {
				descInfo->imageInfos = { {self->getVkSampler(), self->getImageView()->getImageView(), self->getImageView()->getLayout()} };
			}
			else {
				descInfo->imageInfos = { {self->getVkSampler(), NULL, VK_IMAGE_LAYOUT_GENERAL } };
			}
			descInfo->type = self->getVkDescriptorType();
			descInfo->arrCount = 1u;;
			descInfo->pNext = VK_NULL_HANDLE;
		}

		operator const ObjectDescriptorInfo()
		{
			ObjectDescriptorInfo info;
			info.valObject = this;
			info.updateDataCallback = toObjectDescriptorInfo;
			
			toObjectDescriptorInfo(&info);
			return info;
		}

	public:
		VAL_RETURN_CODE create(VkDevice device, bool keepCreateInfo = false);

		void destroy(VkDevice device);
		//void recreate();
	public:
		void bindImageView(ImageView& imageView);

		ImageView* getImageView();
	public:
		void setSamplerType(const SAMPLER_TYPE& type);

		// An anisoLevel greater than 0 will enable anisotropic filtering, if it's equal to 0 it will be disabled.
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

		void useUnnormalizedCoordinates(const bool& val);

		void setCompareMode(VkCompareOp cmpOp/*Set to VK_COMPARE_OP_NEVER to disable*/);

		void setMipLodBias(const float& mipLOD);

		void setFromVkSamplerCreateInfo(const VkSamplerCreateInfo& createInfo);

		void setBorderColor(const VkBorderColor color);

		VkBorderColor getVkBorderColor() const;
	public:

		const SAMPLER_TYPE& getSamplerType()const ;

		const VkSampler& getVkSampler() const;

		VkDescriptorImageInfo& getVkDescriptorImageInfo();

	private:

		// may or may not exist
		std::optional<VkSamplerCreateInfo> getCreateInfo();

		void initDefaultCreateInfoValues();

	protected:
		friend ValProc;

		ImageView* _imgView;

		// VkSampler reference is stored in here
		VkDescriptorImageInfo _VKdescriptorInfo{.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
		SAMPLER_TYPE _samplerType = combinedImage;
	};
}

#endif // !VAL_SAMPLER_HPP
