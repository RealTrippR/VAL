#include <VAL/lib/system/system_utils.hpp>


//#include <VAL/lib/system/image.hpp>

#ifndef VAL_IMAGE_VIEW_HPP
#define VAL_IMAGE_VIEW_HPP
namespace val {
	class image; // forward declaration
	class texture2d; // forward declaration
	class imageView {
	public:
		imageView(VAL_PROC& proc) : _proc(proc) {};
		imageView(VAL_PROC& proc, VkImageLayout* layout) : _proc(proc) { _layout = layout; };
		imageView(VAL_PROC& proc, val::image& img, const VkImageAspectFlags& aspectFlags) : _proc(proc)
		{
			create(img, aspectFlags);
		}

		imageView(VAL_PROC& proc, val::texture2d& texture, const VkImageAspectFlags& aspectFlags) : _proc(proc) 
		{
			create(texture, aspectFlags);
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
		void create(val::image& img, const VkImageAspectFlags& aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

		void create(val::texture2d& texture, const VkImageAspectFlags& aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

		void create(VkImage img, VkFormat format, const VkImageAspectFlags& aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

		void destroy();
	public:
		VAL_PROC& getProc();

		inline VkImageLayout getLayout() {
			return _layout ? *_layout : VK_IMAGE_LAYOUT_UNDEFINED;
		}

		VkImageView& getImageView();

		const VkImageAspectFlags& getAspectFlags();
	protected:
		VAL_PROC& _proc;  // Store a reference
		VkImageLayout* _layout = NULL;
		VkImageView _imgView = VK_NULL_HANDLE;
		VkImageAspectFlags _aspectFlags{};
	};
}

#endif // !VAL_IMAGE_VIEW_HPP