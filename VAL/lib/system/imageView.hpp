#include <VAL/lib/system/system_utils.hpp>


//#include <VAL/lib/system/image.hpp>

#ifndef VAL_IMAGE_VIEW_HPP
#define VAL_IMAGE_VIEW_HPP
namespace val {
	class Image; // forward declaration
	class Texture2D; // forward declaration
	class imageView {
	public:
		imageView(ValProc& proc) : _proc(proc) {};
		imageView(ValProc& proc, VkImageLayout* layout) : _proc(proc) { _layout = layout; };
		imageView(ValProc& proc, val::Image& img, const VkImageAspectFlags& aspectFlags) : _proc(proc)
		{
			create(img, aspectFlags);
		}

		imageView(ValProc& proc, val::Texture2D& texture, const VkImageAspectFlags& aspectFlags) : _proc(proc)
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
		void create(val::Image& img, const VkImageAspectFlags& aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

		void create(val::Texture2D& texture, const VkImageAspectFlags& aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

		void create(VkImage img, VkFormat format, const VkImageAspectFlags& aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

		void destroy();
	public:
		ValProc& getProc();

		inline VkImageLayout getLayout() {
			return _layout ? *_layout : VK_IMAGE_LAYOUT_UNDEFINED;
		}

		VkImageView& getImageView();

		const VkImageAspectFlags& getAspectFlags();
	protected:
		ValProc& _proc;  // Store a reference
		VkImageLayout* _layout = NULL;
		VkImageView _imgView = VK_NULL_HANDLE;
		VkImageAspectFlags _aspectFlags{};
	};
}

#endif // !VAL_IMAGE_VIEW_HPP