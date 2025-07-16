#include <VAL/lib/system/imageView.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void ImageView::create(ImageViewBindInfo& bindInfo, const IMAGE_ASPECT aspectFlags)
	{
		_layout = bindInfo.layout;
		_aspectFlags = (VkImageAspectFlags) aspectFlags;

		if (_imgView != VK_NULL_HANDLE) {
			destroy();
		}

		_proc.createImageView(bindInfo.image, bindInfo.format, (VkImageAspectFlags)aspectFlags, &_imgView);
	}
	
	void ImageView::create(VkImage img, VkFormat format, const IMAGE_ASPECT& aspectFlags)
	{
		if (_imgView != VK_NULL_HANDLE) {
			destroy();
		}
		_aspectFlags = (VkImageAspectFlags)aspectFlags;
		_proc.createImageView(img, format, (VkImageAspectFlags)aspectFlags, &_imgView);
	}

	void ImageView::destroy()
	{
		if (_imgView) {
			vkDestroyImageView(_proc._device, _imgView, VK_NULL_HANDLE);
			_imgView = NULL;
		}
	}

	ValProc& ImageView::getProc()
	{
		return _proc;
	}

	VkImageView& ImageView::getImageView()
	{
		return _imgView;
	}

	const VkImageAspectFlags& ImageView::getAspectFlags()
	{
		return _aspectFlags;
	}
}