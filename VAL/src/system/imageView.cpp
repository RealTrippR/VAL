#include <VAL/lib/system/imageView.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void imageView::create(val::image& img, const VkImageAspectFlags& aspectFlags) {
		_aspectFlags = aspectFlags;
		_img = &img;
		_proc.createImageView(_img->getImage(), _img->getFormat(), VK_IMAGE_ASPECT_COLOR_BIT, &_imgView);
	}

	void imageView::recreate(val::image& img, const VkImageAspectFlags& aspectFlags) {
		destroy();
		create(img, aspectFlags);
	}

	void imageView::destroy() {
		if (_imgView) {
			vkDestroyImageView(_proc._device, _imgView, VK_NULL_HANDLE);
			_imgView = NULL;
		}
	}

	VAL_PROC& imageView::getProc() {
		return _proc;
	}

	val::image& imageView::getImage() {
		return *_img;
	}

	VkImageView& imageView::getImageView() {
		return _imgView;
	}

	const VkImageAspectFlags& imageView::getAspectFlags() {
		return _aspectFlags;
	}
}