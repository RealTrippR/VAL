#include <VAL/lib/system/imageView.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void imageView::create(VAL_PROC& proc, val::image& img, const VkImageAspectFlags& aspectFlags) {
		_proc = proc;
		_aspectFlags = aspectFlags;
		_img = &img;
		proc.createImageView(_img->getImage(), _img->getFormat(), VK_IMAGE_ASPECT_COLOR_BIT, &_imgView);
	}

	void imageView::recreate(VAL_PROC& proc, val::image& img, const VkImageAspectFlags& aspectFlags) {
		destroy();
		create(proc, img, aspectFlags);
	}

	void imageView::destroy() {
		vkDestroyImageView(_proc._device, _imgView, VK_NULL_HANDLE);
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