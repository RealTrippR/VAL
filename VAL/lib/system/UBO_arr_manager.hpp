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

#ifndef VAL_UBO_ARR_MANAGER_HPP
#define VAL_UBO_ARR_MANAGER_HPP

#include <stdint.h>
#include <stdlib.h>

#include <VAL/lib/system/system_utils.hpp>


namespace val {

	class UBO_Handle; // forward declaration

	struct uboArraySubset {
		void create(VAL_PROC& proc, VkBufferUsageFlags additionalUsages, bufferSpace space, UBO_Handle* uboHandles, uint32_t uboCount);
		void destroy(VAL_PROC& proc);
		void* getMappedDataOfFrame(const uint8_t& frameIdx);

		size_t _sizePerFrame = 0; // in bytes
		// data is laid out in a 2d array packed into a fixed, 1d array like so:
		// [frameIndex][uboIndex]
		VkDeviceMemory _vkMem;
		VkBuffer _vkBuff;
		void* _dataMapped;
	};

	struct uboArray {
		void create(VAL_PROC& proc, UBO_Handle** uboHandles, uint32_t uboCount);
		void destroy(VAL_PROC& proc);
		/*************************************/
		/* GPU-only memory                   */
		uboArraySubset localReadOnly;
		uboArraySubset localDstTransferOnly;
		uboArraySubset localSrcTransferOnly;
		uboArraySubset localDstAndSrcTransfer;

		// optional, will be used if any uboHandle has one of the following flags:
		// VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		// VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, (yes, it's technically possible to have a UBO that dual purposes as an SSBO, but this is a bad design practice.)
		// VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
		uboArraySubset localOther;

		/*************************************/
		/* GPU-CPU memory                    */
		uboArraySubset globalReadOnly;
		uboArraySubset globalDstTransferOnly;
		uboArraySubset globalSrcTransferOnly;
		uboArraySubset globalDstAndSrcTransfer;

		// optional, will be used if any uboHandle has one of the following flags:
		// VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		// VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, (yes, it's technically possible to have a UBO that dual purposes as an SSBO, but this is a bad design practice.)
		// VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
		uboArraySubset globalOther;
	};
}

#endif //!VAL_UBO_ARR_MANAGER_HPP