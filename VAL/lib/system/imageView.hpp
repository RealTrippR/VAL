#include <val/lib/system/system_utils.hpp>

#ifndef VAL_IMAGE_VIEW_HPP
#define VAL_IMAGE_VIEW_HPP
namespace val {
	class image; // forward declaration

	class imageView {
	public:
		imageView(VAL_PROC& proc) : _proc(proc) {};
		imageView(VAL_PROC& proc, val::image& img, const VkImageAspectFlags& aspectFlags) : _proc(proc) {
			create(img, aspectFlags);
		}

		~imageView() {
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
		void create(val::image& img, const VkImageAspectFlags& aspectFlags);

		void recreate(val::image& img, const VkImageAspectFlags& aspectFlags);

		void destroy();
	public:
		VAL_PROC& getProc();

		val::image& getImage();

		VkImageView& getImageView();

		const VkImageAspectFlags& getAspectFlags();
	protected:
		VAL_PROC& _proc;  // Store a reference
		val::image* _img = NULL;
		VkImageView _imgView = VK_NULL_HANDLE;
		VkImageAspectFlags _aspectFlags{};
	};
}

#endif // !VAL_IMAGE_VIEW_HPP