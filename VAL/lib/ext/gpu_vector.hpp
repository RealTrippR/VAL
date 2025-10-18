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
#include <algorithm>

#ifndef VAL_GPU_VECTOR_HPP
#define VAL_GPU_VECTOR_HPP

namespace val {
	template<typename T, typename size_t = uint32_t>
	class gpu_vector {
	public:

		gpu_vector() = default;

		gpu_vector(const VkBufferUsageFlags usage) 
		{
			_usage = usage;
		}

		gpu_vector(const BUFFER_USAGE usage)
		{
			_usage = (VkBufferUsageFlags)usage;
		}

		gpu_vector(ValProc& proc, const VkBufferUsageFlags usage) {
			_usage = usage;
			init(proc, 0);
		}


		gpu_vector(ValProc& proc, const BUFFER_USAGE usage) {
			_usage = (VkBufferUsageFlags)usage;
			init(proc, 0);
		}


		gpu_vector(ValProc& proc, const VkBufferUsageFlags usage, size_t size, const T& val) {
			_usage = usage;
			init(proc, size);

			for (size_t i = 0; i < size; ++i) {
				_mappedMemory[i] = val;
			}
		}

		gpu_vector(ValProc& proc, const BUFFER_USAGE usage, size_t size, const T& val) {
			_usage = (VkBufferUsageFlags)usage;
			init(proc, size);

			for (size_t i = 0; i < size; ++i) {
				_mappedMemory[i] = val;
			}
		}

		gpu_vector(ValProc& proc, const VkBufferUsageFlags usage, std::initializer_list<T> list) {
			_usage = usage;
			init(proc, list.size());

				
			size_t i = 0;
			for (const T& value : list) {
				_mappedMemory[i++] = value;
				T tmp = _mappedMemory[i-1];
				const int a=0u;
			}
		}

		gpu_vector(ValProc& proc, const BUFFER_USAGE usage, std::initializer_list<T> list) {
			_usage = (VkBufferUsageFlags)usage;
			init(proc, list.size());


			size_t i = 0;
			for (const T& value : list) {
				_mappedMemory[i++] = value;
				T tmp = _mappedMemory[i - 1];
				const int a = 0u;
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

		// Overload for non-const objects (allows modification)
		T& operator[](size_t index) {
#ifndef NDEBUG
			if (index >= _size) {
				throw std::out_of_range("Invalid gpu_vector write access index.");
			}
#endif
			return data()[index];
		}
		// Overload for const objects (read-only access)
		const T& operator[](size_t index) const {
#ifndef NDEBUG
			if (index >= _size) {
				throw std::out_of_range("Invalid gpu_vector read access index.");
			}
#endif
			return data()[index];
		}

		operator const VkBuffer() const {
			return _buffer;
		}

		operator const VkBuffer&() const {
			return _buffer;
		}

		operator VkBuffer() {
			return _buffer;
		}

		operator VkBuffer&() {
			return _buffer;
		}
	public:

		inline T& front()
		{
			return data()[0];
		}

		inline T& back() 
		{
			return data()[size()-1];
		}
	public:

		inline VkDeviceMemory& getVkDeviceMemory() {
			return _memory;
		}

		inline VkBuffer& getVkBuffer() {
			return _buffer;
		}

		inline void push_back(Queue& q, const T& obj)
		{
			resize(q, size() + 1);
			data()[size() - 1] = obj;
			T& t = back();
			int i = 0;
		}

		inline void push_back(Queue& q, const T& obj, VkCommandBuffer cmdBuff)
		{
			resize(q, size() + 1, cmdBuff);
			data()[size() - 1] = obj;
		}

		inline void pop_back(Queue& q) 
		{
			if (size() > 0) {
				resize(q, size() - 1);
			}
		}

		inline void pop_back(Queue& q, VkCommandBuffer cmdBuff)
		{
			if (size() > 0) {
				resize(q, size() - 1, cmdBuff);
			}
		}

		inline void resize(Queue& q, const size_t& newSize) 
		{
			auto& proc = *q.getValProc();
			if (_size == newSize) { return; }
			if (this->size() == 0) { init(proc, newSize); return; }


			size_t tmp_capacity = roundToNextPowerOfTwo(newSize + 1);

			// don't alloc or dealloc data, just call constructors
			if (tmp_capacity == _capacity)
			{
				if (newSize > _size) {
					callConstructors(_size, newSize - 1);
				}
				else {
					callDestructors(newSize, _size - 1);
				}
				_size = newSize;
				return;
			}

			VkBuffer newBuffer;
			VkDeviceMemory newMemory;
			T* newMappedMemory;

			const VkDeviceSize buffSzeInbytes = tmp_capacity * sizeof(T);

			proc.createBufferAutoMemoryAllocFlags(buffSzeInbytes, _usage, bufferSpaceToVkMemoryProperty(CPU_GPU), newBuffer, newMemory);

			VkDeviceMemory oldMemory = _memory;
			VkBuffer oldBuffer = _buffer;

			if (newMemory)
			{ // check if alloc succeeded
				proc.copyBuffer(q,q.getCmdPool(), _buffer, newBuffer, _size * sizeof(T), 0u, 0u);

				_memory = newMemory;
				_buffer = newBuffer;

				vkMapMemory(proc._device, newMemory, 0, buffSzeInbytes, 0, (void**) & newMappedMemory);

				std::copy(_mappedMemory, _mappedMemory + _size, newMappedMemory);
#ifndef NDEBUG
				if (newMappedMemory == nullptr) { throw std::runtime_error("gpu_vector: Failed to map newMappedMemory."); }
#endif // !NDEBUG
				_mappedMemory = newMappedMemory;

				// destroy the old buffer
				if (oldMemory) {
					vkFreeMemory(proc._device, oldMemory, VK_NULL_HANDLE);
					vkDestroyBuffer(proc._device, oldBuffer, VK_NULL_HANDLE);
				}

				if (newSize > _size) {
					callConstructors(_size, newSize - 1);
				}
				else {
					callDestructors(newSize, _size - 1);
				}

				_size = newSize;
				_capacity = tmp_capacity;
			}
		}

		// destroys the old memory and creates a new buffer - no copying will take place.
		inline void setUsagesNoCopy(ValProc& proc, const VkBufferUsageFlags newUsages) 
		{
			if (_size > 0) {
				destroy(proc);
				init(proc, _size);
			}
			_usage = newUsages;
		}

		// destroys the old memory and creates a new buffer
		inline void setUsages(ValProc& proc, const VkBufferUsageFlags newUsages)
		{
			if (_size > 0) {
				gpu_vector<T, size_t> newSelf;
				newSelf.init(proc, _size);
				newSelf._usage = newUsages;
				VkCommandBuffer cmd = proc.beginSingleTimeCommands();
				copy(proc, newSelf, cmd);
				proc.endSingleTimeCommands(cmd);
				destroy(proc);
				*this = newSelf;
			}
			else {
				_usage = newUsages;
			}
		}
		
		// copies data from this into other
		inline void copy(ValProc& proc, VkBuffer& dst, uint32_t otherSize, VkCommandBuffer cmdBuff, void* dstMappedMemory)
		{
#ifndef NDEBUG
			if (dst == NULL) {
				throw std::runtime_error("gpu_vector::copy: `dst` is an invalid/uninitialized buffer.");
			}
#endif
			VkBufferCopy copyRegion;
			copyRegion.srcOffset = 0u; 
			copyRegion.dstOffset = 0u;
			copyRegion.size = _size * sizeof(T);
#ifndef NDEBUG
			if (otherSize < copyRegion.size) {
				throw std::runtime_error("Failed to copy buffer: src buffer is larger than dst buffer!");
			}
#endif // !NDEBUG

		
			if (dstMappedMemory && _mappedMemory) {
				std::copy(_mappedMemory, _mappedMemory + _size, dstMappedMemory);
			}
			else {
				vkCmdCopyBuffer(cmdBuff, _buffer, dst, 1, &copyRegion);
			}
		}
		// copies  data from this into other
		inline void copy(Queue& q, gpu_vector<T, size_t>& dst, VkCommandBuffer cmdBuff) {
#ifndef NDEBUG
			if (dst._buffer == NULL) {
				throw std::runtime_error("gpu_vector::copy: `dst` is an invalid/uninitialized gpu_vector.");
			}
#endif
			if (dst._size < _size) {
				dst.resize(q, _size);
			}

			VkBufferCopy copyRegion;
			copyRegion.srcOffset = 0u;
			copyRegion.dstOffset = 0u;
			copyRegion.size = _size * sizeof(T);

			if (dst._mappedMemory && _mappedMemory) {
				std::copy(_mappedMemory, _mappedMemory + _size, dst._mappedMemory);
			}
			else {
				vkCmdCopyBuffer(cmdBuff, _buffer, dst.getVkBuffer(), 1, &copyRegion);
			}
		}

		inline void destroy(ValProc& proc) {
			if (_memory) {
				callDestructors(0, _size-1);

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

		T* data(size_t i) {
			return (T*)_mappedMemory+i;
		}

		inline bool empty() const
		{
			return _size == 0;
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

		void callConstructors(size_t idxBegin, size_t idxEnd)
		{

			for (size_t i = idxBegin; i <= idxEnd; ++i)
			{
				new (data(i))  T();
			}
		}

		void callDestructors(size_t idxBegin, size_t idxEnd)
		{
			for (size_t i = idxBegin; i <= idxEnd; ++i)
			{
				data(i)->~T();
			}
		}

		inline void init(ValProc& proc, size_t size)
		{
			if (size == 0) { 
				return;
			}
			_size = size;

			_capacity = roundToNextPowerOfTwo(_size + 1);
			const VkDeviceSize buffSzeInbytes = _capacity * sizeof(T);

			proc.createBufferAutoMemoryAllocFlags(buffSzeInbytes, _usage, bufferSpaceToVkMemoryProperty(CPU_GPU), _buffer, _memory);

			vkMapMemory(proc._device, _memory, 0, _size, 0, (void**)&_mappedMemory);
			
			callConstructors(0, size-1);
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