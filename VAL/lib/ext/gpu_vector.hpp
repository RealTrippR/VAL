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

#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/ext/roundToNextPowerOf2.hpp>
#include <initializer_list>
#include <stdexcept>

#ifndef VAL_GPU_VECTOR_HPP
#define VAL_GPU_VECTOR_HPP

namespace val {
	template<typename T, typename size_t = uint32_t>
	class gpu_vector {
	public:
		gpu_vector(VAL_PROC& proc, const VkBufferUsageFlags& usage) {
			init(proc, 0);
		}


		gpu_vector(VAL_PROC& proc, const VkBufferUsageFlags& usage, size_t size, const T& val) {
			_usage = usage;
			init(proc, size);

			for (size_t i = 0; i < size; ++i) {
				_mappedMemory[i] = val;
			}
		}

		gpu_vector(VAL_PROC& proc, const VkBufferUsageFlags& usage, std::initializer_list<T> list) {
			_usage = usage;
			init(proc, list.size());

				
			size_t i = 0;
			for (const T& value : list) {
				_mappedMemory[i++] = value;
				T tmp = _mappedMemory[i-1];
				const int a=0u;
			}
			
		}

		
		~gpu_vector() {
#ifndef NBDEUG
			if (_memory) {
				val::dbg::printError("Gpu_vector at address %h has not been properly destroyed.\n", this);
				throw std::runtime_error("VAL: ERROR: A gpu_vector has not been properly destroyed");
			}
#endif // !NBDEUG
		}

		operator VkBuffer() {
			return _buffer;
		}

		operator VkBuffer&() {
			return _buffer;
		}
	public:

		inline VkDeviceMemory& getVkDeviceMemory() {
			return _memory;
		}

		inline VkBuffer& getVkBuffer() {
			return _buffer;
		}

		inline void init(VAL_PROC& proc, size_t size) {
			_size = size;

			_capacity = roundToNextPowerOfTwo(_size + 1);
			const VkDeviceSize buffSzeInbytes = _capacity * sizeof(T);


			proc.createBuffer(buffSzeInbytes, _usage, bufferSpaceToVkMemoryProperty(CPU_GPU), _buffer, _memory);

			vkMapMemory(proc._device, _memory, 0, _size, 0, (void**)&_mappedMemory);
		}

		inline void resize(VAL_PROC& proc, const size_t& newSize) {
			if (_size == newSize) { return; }

			VkBuffer newBuffer;
			VkDeviceMemory newMemory;

			size_t tmp_capacity = roundToNextPowerOfTwo(newSize + 1);

			const VkDeviceSize buffSzeInbytes = tmp_capacity * sizeof(T);

				
			proc.createBuffer(buffSzeInbytes, _usage, bufferSpaceToVkMemoryProperty(CPU_GPU), newBuffer, newMemory);

			if (newMemory) { // check if alloc succeeded
				proc.copyBuffer(_buffer, newBuffer, buffSzeInbytes, 0u, 0u);

				// destroy the old buffer
				if (_memory) {
					vkFreeMemory(proc._device, _memory, VK_NULL_HANDLE);
					vkDestroyBuffer(proc._device, _buffer, VK_NULL_HANDLE);
					_memory = VK_NULL_HANDLE;
					_buffer = VK_NULL_HANDLE;
				}
					
				_memory = newMemory;
				_buffer = newBuffer;

				_size = newSize;
				_capacity = tmp_capacity;

				vkMapMemory(proc._device, _memory, 0, _size, 0, (void**)& _mappedMemory);
			}
		}

		inline void resize(VAL_PROC& proc, const size_t& newSize, VkCommandBuffer& commandBuff) {
			if (_size == newSize) { return; }

			VkBuffer newBuffer;
			VkDeviceMemory newMemory;

			size_t tmp_capacity = roundToNextPowerOfTwo(newSize + 1);

			const VkDeviceSize buffSzeInbytes = tmp_capacity * sizeof(T);


			proc.createBuffer(buffSzeInbytes, _usage, bufferSpaceToVkMemoryProperty(CPU_GPU), newBuffer, newMemory);

			if (newMemory) { // check if alloc succeeded
				

				// destroy the old buffer
				if (_memory) {
					vkFreeMemory(proc._device, _memory, VK_NULL_HANDLE);
					vkDestroyBuffer(proc._device, _buffer, VK_NULL_HANDLE);
					_memory = VK_NULL_HANDLE;
					_buffer = VK_NULL_HANDLE;
				}

				_memory = newMemory;
				_buffer = newBuffer;

				_size = newSize;
				_capacity = tmp_capacity;

				vkMapMemory(proc._device, _memory, 0, _size, 0, &_mappedMemory);
			}
		}

		inline void copy(VAL_PROC& proc, VkBuffer& other, uint32_t otherSize, VkCommandBuffer& cmdBuff) {

			VkBufferCopy copyRegion;
			copyRegion.srcOffset = 0u; 
			copyRegion.dstOffset = 0u;
			copyRegion.size = _size * sizeof(T);
#ifndef NDEBUG
			if (otherSize < copyRegion.size) {
				throw std::runtime_error("Failed to copy buffer: src buffer is larger than dst buffer!");
			}
#endif // !NDEBUG

		

			vkCmdCopyBuffer(cmdBuff, _buffer, other, 1, &copyRegion);
		}


		inline void copy(VAL_PROC& proc, gpu_vector& other, VkCommandBuffer& cmdBuff) {

			if (other._size < _size) {
				other.resize(proc, _size);
			}

			VkBufferCopy copyRegion;
			copyRegion.srcOffset = 0u;
			copyRegion.dstOffset = 0u;
			copyRegion.size = _size * sizeof(T);
			vkCmdCopyBuffer(cmdBuff, _buffer, other._buffer, 1, &copyRegion);
		}

		inline void destroy(VAL_PROC& proc) {
			if (_memory) {
				vkDestroyBuffer(proc._device, _buffer, VK_NULL_HANDLE);
				vkFreeMemory(proc._device, _memory, VK_NULL_HANDLE);
				_size = 0u;
				_capacity = 0u;
				_mappedMemory = NULL;
				_memory = NULL;
			}
		}

	public:
		T* data() {
			return (T*)_mappedMemory;
		}

		const size_t& size() const {
			return _size;
		}

		const size_t& capacity() const {
			return _capacity;
		}

		const VkDeviceSize& getSizeInBytes() const {
			return _size * sizeof(T);
		}

		const VkDeviceSize& sizeInBytes() const {
			return _size * sizeof(T);
		}

		const VkBuffer& getVkBuffer() const {
			return _buffer;
		}

		const VkDeviceMemory& getVkMemory() const {
			return _memory;
		}

		const VkBufferUsageFlags& getUsage() const {
			return _usage;
		}

	private:
		VkBufferUsageFlags _usage = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
		size_t _capacity = 0u;
		size_t _size = 0u;
		VkDeviceMemory _memory = VK_NULL_HANDLE;
		VkBuffer _buffer = VK_NULL_HANDLE;

		T* _mappedMemory = NULL;
	};
}

#endif // !VAL_GPU_VECTOR_HPP