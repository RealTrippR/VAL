/*
Copyright © 2025 Tripp Robins

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the “Software”), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <VAL/lib/system/texture2d.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void texture2d::destroy()
	{
		if (_imgMemory) {
			_imgMemory = VK_NULL_HANDLE;
			vkFreeMemory(_proc._device, _imgMemory, VK_NULL_HANDLE);
		}
		if (_img) {
			_img = VK_NULL_HANDLE;
			vkDestroyImage(_proc._device, _img, VK_NULL_HANDLE);
		}
		if (_pixels) {
			stbi_image_free(_pixels);
			_pixels = NULL;
		}
	}

	void texture2d::create(std::filesystem::path srcpath, const VkFormat format, const VkImageUsageFlagBits usages,
		const VkImageLayout layout, const bufferSpace memspace, const uint8_t mipLevels)
	{

#ifndef NDEBUG
		if (mipLevels == 0u) {
			dbg::printError("Mip levels of texture2d img must not be 0.");
		}
#endif // !NDEBUG
		destroy();

		_format = format;
		_mipLevels = mipLevels;

		int widthtmp;
		int heightmp;
		_img = createTextureImage(&_proc, srcpath, &_pixels, _format, _imgMemory,
			VkImageUsageFlagBits(0), _mipLevels, &widthtmp, &heightmp, &_channels, memspace);
		_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		_width = widthtmp;
		_height = heightmp;

		if (mipLevels > 0) {
			generateMipmaps(mipLevels);
		}
	}

	void texture2d::create(const uint16_t width, const uint16_t height, const VkFormat format, const VkImageUsageFlagBits usages, 
		const VkImageLayout layout, const bufferSpace memspace, const uint8_t mipLevels)
	{
		destroy();


#ifndef NDEBUG
		if (mipLevels == 0u) {
			dbg::printError("Mip levels of texture2d img must not be 0.");
		}
#endif // !NDEBUG

		_layout = layout;
		_width = width;
		_height = height;
		_format = format;
		_mipLevels = mipLevels;
		_proc.createImage(width, height, format, VK_IMAGE_TILING_OPTIMAL, usages, memspace, _img, _imgMemory, mipLevels, VK_SAMPLE_COUNT_1_BIT, _layout);
	}

	/* PRIVATE: */

	void texture2d::generateMipmaps(const uint8_t mipLevels)
	{
#ifndef NDEBUG
		// Check if image format supports linear blitting
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(_proc._physicalDevice, _format, &formatProperties);

		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			dbg::printError("Texture image format does not support linear blitting!");
			throw std::runtime_error("Texture image format does not support linear blitting!");
		}
#endif // !NDEBUG



		VkCommandBuffer commandBuffer = _proc.beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = _img;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = _width;
		int32_t mipHeight = _height;

		for (uint32_t i = 1; i < mipLevels; i++) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer,
				_img, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				_img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		_proc.endSingleTimeCommands(commandBuffer);
	}
}