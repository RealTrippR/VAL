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

#include <VAL/lib/system/UBO_arr_manager.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {

	/***************************************************/
	/* UBO ARRAY SUBSET */

	void uboArraySubset::create(VAL_PROC& proc, VkBufferUsageFlags additionalUsages, bufferSpace space,
		UBO_Handle* uboHandles, uint32_t uboCount)
	{
		if (uboCount == 0u) {
			return;
		}


		size_t& sizePerFrame = _sizePerFrame;
		sizePerFrame = 0u;
		// first calculate the size per frame and init UBOS
		for (uint32_t i = 0; i < uboCount; ++i) {
			UBO_Handle& ubo = uboHandles[i];
			ubo._offset = sizePerFrame;
			sizePerFrame += ubo._size;

			ubo._arrSubset = this;
		}

		const uint8_t frameCount = proc._MAX_FRAMES_IN_FLIGHT;
		const uint64_t totalSize = sizePerFrame * frameCount;

		proc.createBuffer(totalSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | additionalUsages, bufferSpaceToVkMemoryProperty(space), _vkBuff, _vkMem);

		if (space == CPU_GPU) {
			vkMapMemory(proc._device, _vkMem, 0, totalSize, 0, &_dataMapped);
		}

	}

	void uboArraySubset::destroy(VAL_PROC& proc) {
		if (_vkBuff) {
			vkFreeMemory(proc._device, _vkMem, NULL);
			vkDestroyBuffer(proc._device, _vkBuff, NULL);
		}
		_vkBuff = NULL;
		_vkMem = NULL;
	}

	void* uboArraySubset::getMappedDataOfFrame(const uint8_t& frameIdx) {
		return (void*)((uint8_t*)_dataMapped + frameIdx * _sizePerFrame);
	}
	
	/***************************************************/
	/* UBO ARRAY */

	void uboArray::create(VAL_PROC& proc, UBO_Handle* uboHandles, uint32_t uboCount) {
	
		{ // create host local buffers (GPU Only)
			VkBufferUsageFlags optionalFlags = 0x0;

			std::vector<UBO_Handle*> validLocalReadOnly;
			std::vector<UBO_Handle*> validLocalDstTransferOnly;
			std::vector<UBO_Handle*> validLocalSrcTransferOnly;
			std::vector<UBO_Handle*> validLocalDstAndSrcTransfer;
			std::vector<UBO_Handle*> validLocalOther;
			for (uint32_t i = 0; i < uboCount; ++i) {
				UBO_Handle* Hdl = &uboHandles[i];
				if (Hdl->_space != GPU_ONLY) {
					continue; // device local space only
				}
				switch (Hdl->_additionalUsageFlags) {
				case 0x0:
					validLocalReadOnly.push_back(Hdl);
					break;
				case VK_BUFFER_USAGE_TRANSFER_DST_BIT:
					validLocalDstTransferOnly.push_back(Hdl);
					break;
				case VK_BUFFER_USAGE_TRANSFER_SRC_BIT:
					validLocalSrcTransferOnly.push_back(Hdl);
					break;
				case (VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT):
					validLocalDstAndSrcTransfer.push_back(Hdl);
					break;
				default:
					validLocalOther.push_back(Hdl);
					optionalFlags |= Hdl->_additionalUsageFlags;
				}
			}
			if (!validLocalReadOnly.empty()) {
				localReadOnly.create(proc, 0x0, GPU_ONLY, *(validLocalReadOnly.data()), validLocalReadOnly.size());
			}
			if (!validLocalDstTransferOnly.empty()) {
				localDstTransferOnly.create(proc, VK_BUFFER_USAGE_TRANSFER_DST_BIT, GPU_ONLY, *(validLocalDstTransferOnly.data()), validLocalDstTransferOnly.size());
			}
			if (!validLocalSrcTransferOnly.empty()) {
				localSrcTransferOnly.create(proc, VK_BUFFER_USAGE_TRANSFER_DST_BIT, GPU_ONLY, *(validLocalSrcTransferOnly.data()), validLocalSrcTransferOnly.size());
			}
			if (!validLocalDstAndSrcTransfer.empty()) {
				localDstAndSrcTransfer.create(proc, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, GPU_ONLY, *(validLocalDstAndSrcTransfer.data()), validLocalDstAndSrcTransfer.size());
			}

			// there is no reason to create the other array subset if it's not needed
			if (optionalFlags == (VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)
				|| optionalFlags == 0x0) {
			}
			else {
				if (!validLocalOther.empty()) {
					localOther.create(proc, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | optionalFlags, GPU_ONLY, *(validLocalOther.data()), validLocalOther.size());
				}
			}
		}

		{ // create host global buffers (GPU-CPU)
			VkBufferUsageFlags optionalFlags = 0x0;

			std::vector<UBO_Handle*> validGlobalReadOnly;
			std::vector<UBO_Handle*> validGlobalDstTransferOnly;
			std::vector<UBO_Handle*> validGlobalSrcTransferOnly;
			std::vector<UBO_Handle*> validGlobalDstAndSrcTransfer;
			std::vector<UBO_Handle*> validGlobalOther;
			for (uint32_t i = 0; i < uboCount; ++i) {
				UBO_Handle* Hdl = &uboHandles[i];
				if (Hdl->_space != CPU_GPU) {
					continue; // device local space only
				}
				switch (Hdl->_additionalUsageFlags) {
				case 0x0:
					validGlobalReadOnly.push_back(Hdl);
					break;
				case VK_BUFFER_USAGE_TRANSFER_DST_BIT:
					validGlobalDstTransferOnly.push_back(Hdl);
					break;
				case VK_BUFFER_USAGE_TRANSFER_SRC_BIT:
					validGlobalSrcTransferOnly.push_back(Hdl);
					break;
				case (VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT):
					validGlobalDstAndSrcTransfer.push_back(Hdl);
					break;
				default:
					validGlobalOther.push_back(Hdl);
					optionalFlags |= Hdl->_additionalUsageFlags;
				}
			}
			if (!validGlobalReadOnly.empty()) {
				globalReadOnly.create(proc, 0x0, CPU_GPU, *(validGlobalReadOnly.data()), validGlobalReadOnly.size());
			}
			if (!validGlobalDstTransferOnly.empty()) {
				globalDstTransferOnly.create(proc, VK_BUFFER_USAGE_TRANSFER_DST_BIT, CPU_GPU, *(validGlobalDstTransferOnly.data()), validGlobalDstTransferOnly.size());
			}
			if (!validGlobalSrcTransferOnly.empty()) {
				globalSrcTransferOnly.create(proc, VK_BUFFER_USAGE_TRANSFER_DST_BIT, CPU_GPU, *(validGlobalSrcTransferOnly.data()), validGlobalSrcTransferOnly.size());
			}
			if (!validGlobalOther.empty()) {
				globalDstAndSrcTransfer.create(proc, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, CPU_GPU, *(validGlobalDstAndSrcTransfer.data()), validGlobalDstAndSrcTransfer.size());
			}

			// there is no reason to create the other array subset if it's not needed
			if (optionalFlags == (VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)
				|| optionalFlags == 0x0) {
			}
			else {
				globalOther.create(proc, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | optionalFlags, CPU_GPU, *(validGlobalOther.data()), validGlobalOther.size());
			}

		}
	}

	void uboArray::destroy(VAL_PROC& proc) {
		localReadOnly.destroy(proc);
		localDstTransferOnly.destroy(proc);
		localSrcTransferOnly.destroy(proc);
		localDstAndSrcTransfer.destroy(proc);
		localOther.destroy(proc);

		globalReadOnly.destroy(proc);
		globalDstTransferOnly.destroy(proc);
		globalSrcTransferOnly.destroy(proc);
		globalDstAndSrcTransfer.destroy(proc);
		globalOther.destroy(proc);
	}
}