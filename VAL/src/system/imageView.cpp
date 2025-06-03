#include <VAL/lib/system/imageView.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void imageView::create(val::image& img, const VkImageAspectFlags& aspectFlags)
	{
		_layout = &img._imgLayout;
		if (_imgView != VK_NULL_HANDLE) {
			destroy();
		}
		_aspectFlags = aspectFlags;
		_proc.createImageView(img.getImage(), img.getFormat(), aspectFlags, &_imgView);
	}


	void imageView::create(val::texture2d& texture, const VkImageAspectFlags& aspectFlags) 
	{
		_layout = &(texture._layout);

		if (_imgView != VK_NULL_HANDLE) {
			destroy();
		}
		_aspectFlags = aspectFlags;
		_proc.createImageView(texture.getVkImage(), texture.getVkFormat(), aspectFlags, &_imgView);
	}

	void imageView::create(VkImage img, VkFormat format, const VkImageAspectFlags& aspectFlags)
	{
		if (_imgView != VK_NULL_HANDLE) {
			destroy();
		}
		_aspectFlags = aspectFlags;
		_proc.createImageView(img, format, aspectFlags, &_imgView);
	}

	void imageView::destroy() 
	{
		if (_imgView) {
			vkDestroyImageView(_proc._device, _imgView, VK_NULL_HANDLE);
			_imgView = NULL;
		}
	}

	VAL_PROC& imageView::getProc() 
	{
		return _proc;
	}

	VkImageView& imageView::getImageView() 
	{
		return _imgView;
	}

	const VkImageAspectFlags& imageView::getAspectFlags() 
	{
		return _aspectFlags;
	}
}