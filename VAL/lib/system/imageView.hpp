#include <VAL/lib/system/system_utils.hpp>


//#include <VAL/lib/system/image.hpp>

#ifndef VAL_IMAGE_VIEW_HPP
#define VAL_IMAGE_VIEW_HPP
namespace val {
	class Image; // forward declaration
	class Texture2D; // forward declaration
	class ImageView 
	{
	public:
		ImageView(ValProc& proc) : _proc(proc) {};
		ImageView(ValProc& proc, VkImageLayout* layout) : _proc(proc) { _layout = layout; };
		ImageView(ValProc& proc, ImageViewBindInfo bindInfo, const VkImageAspectFlags& aspectFlags, const VkDescriptorType type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE) : _proc(proc)
		{
			_type = type;
			create(bindInfo, aspectFlags);
		}

		ImageView(ValProc& proc, VkImage img,VkFormat format, const VkImageAspectFlags& aspectFlags, const VkDescriptorType type= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE) : _proc(proc)
		{
			_type = type;
			create(img,format, aspectFlags);
		}

		~ImageView() {
			destroy();
		}
		// conversion operator to VkImageView
		operator VkImageView() const {
			return _imgView;
		}
		// conversion operator to VkImageView*
		operator VkImageView* () {
			return &_imgView;
		}

	public:
		inline VkDescriptorType getVkDescriptorType() {
			return _type;
		}

		inline static void toObjectDescriptorInfo(ObjectDescriptorInfo* descInfo)
		{
			ImageView* self = (ImageView*)descInfo->valObject;
			descInfo->imageInfos = { VkDescriptorImageInfo{VK_NULL_HANDLE, self->getImageView(),self->getLayout()}};
			descInfo->type = self->getVkDescriptorType();
			descInfo->arrCount = 1u;
			descInfo->pNext = VK_NULL_HANDLE;
		}

		operator const ObjectDescriptorInfo()
		{
			ObjectDescriptorInfo info{
				.valObject = this ,
				.updateDataCallback = toObjectDescriptorInfo
			};
			toObjectDescriptorInfo(&info);
			return info;
		}
	public:

		void create(ImageViewBindInfo& bindInfo, const VkImageAspectFlags aspectFlags);

		void create(VkImage img, VkFormat format, const VkImageAspectFlags& aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

		void destroy();
	public:
		ValProc& getProc();

		inline VkImageLayout getLayout() {
			return _layout ? *_layout : VK_IMAGE_LAYOUT_UNDEFINED;
		}

		VkImageView& getImageView();

		const VkImageAspectFlags& getAspectFlags();

		inline VkDescriptorType getType() { return _type; };

		inline void setType(const VkDescriptorType type) { _type = type; };
	protected:
		ValProc& _proc;  // Store a reference
		VkImageLayout* _layout = NULL;
		VkImageView _imgView = VK_NULL_HANDLE;
		VkImageAspectFlags _aspectFlags{};
		VkDescriptorType _type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	};
}

#endif // !VAL_IMAGE_VIEW_HPP