#include <VAL/lib/system/imageView.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void ImageView::create(ImageViewBindInfo& bindInfo, const VkImageAspectFlags aspectFlags)
	{
		_layout = bindInfo.layout;
		_aspectFlags = aspectFlags;

		if (_imgView != VK_NULL_HANDLE) {
			destroy();
		}

		_proc.createImageView(bindInfo.image, bindInfo.format, aspectFlags, &_imgView);
	}
	/*void ImageView::create(val::Image& img, const VkImageAspectFlags& aspectFlags)
	{
		_layout = &img._imgLayout;
		if (_imgView != VK_NULL_HANDLE) {
			destroy();
		}
		_aspectFlags = aspectFlags;
		_proc.createImageView(img.getImage(), img.getFormat(), aspectFlags, &_imgView);
	}


	void ImageView::create(val::Texture2D& texture, const VkImageAspectFlags& aspectFlags)
	{
		_layout = &(texture._layout);

		if (_imgView != VK_NULL_HANDLE) {
			destroy();
		}
		_aspectFlags = aspectFlags;
		_proc.createImageView(texture.getVkImage(), texture.getVkFormat(), aspectFlags, &_imgView);
	}*/

	void ImageView::create(VkImage img, VkFormat format, const VkImageAspectFlags& aspectFlags)
	{
		if (_imgView != VK_NULL_HANDLE) {
			destroy();
		}
		_aspectFlags = aspectFlags;
		_proc.createImageView(img, format, aspectFlags, &_imgView);
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