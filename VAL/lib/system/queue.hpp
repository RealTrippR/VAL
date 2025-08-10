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


#ifndef VAL_QUEUE_HPP
#define VAL_QUEUE_HPP

#include <VAL/lib/system/queueFlags.hpp>
#include <VAL/lib/system/system_utils.hpp>

namespace val 
{
	class Queue 
	{
	public:
		Queue() = default;
		Queue(ValProc* proc) : _proc(proc)
		{
			if (proc != NULL) {
				setCmdBuffAndSemaphoreCountFromProc(*proc);
				create();
			}
		}
		Queue(ValProc& proc) : _proc(&proc) 
		{
			setCmdBuffAndSemaphoreCountFromProc(proc);
			create();
		}

		Queue(ValProc& proc, const QUEUE_FLAGS flags) : _proc(&proc)
		{
			_queueFlags = flags;
			setCmdBuffAndSemaphoreCountFromProc(proc);
			create();
		}

		Queue(ValProc& proc, const VkQueueFlags flags) : _proc(&proc)
		{
			_queueFlags = (QUEUE_FLAGS)flags;
			setCmdBuffAndSemaphoreCountFromProc(proc);

			create();
		}

		Queue(Queue& other)
		{
			copyToOther(&other);
		}

		Queue& operator=(const Queue& other) {
			other.copyToOther(this);
			return *this;
		}

		operator VkCommandBuffer () const {
			return getCommandBuffer();
		}

		~Queue() {
			destroy();
		}

	public:
		operator const VkQueue&() const {
			return _vkQueue;
		}

		operator VkQueue() {
			return _vkQueue;
		}

	public:
		inline void reset();

		inline void begin();

		inline void end();

		inline void submit(Queue& waitUpon, const VkPipelineStageFlags& waitStages);

		inline void submit(Queue& waitUpon, const VkPipelineStageFlags& waitStages, const VkFence& fence);

		inline void submit(const VkSemaphore* waitSemaphores, const uint8_t waitSemaphoreCount, const VkPipelineStageFlags* waitStages /*1 for every semaphore*/);

		inline void submit(const VkSemaphore* waitSemaphores, const uint8_t waitSemaphoreCount, const VkPipelineStageFlags* waitStages /*1 for every semaphore*/, const VkFence& fence);

		inline void submit(const VkSemaphore* waitSemaphores, const uint8_t waitSemaphoreCount, const VkPipelineStageFlags* waitStages /*1 for every semaphore*/, const VkFence& fence, uint8_t frameIdx, bool signal);

	public:
		inline uint8_t getCommandBufferCount() const;

		inline uint8_t getSemaphoreCount() const;

		inline VkCommandBuffer& getCommandBuffer() const;

		inline VkCommandBuffer& getCommandBuffer(const uint8_t frameIdx) const;

		inline VkSemaphore& getSemaphore() const;

		inline VkSemaphore& getSemaphore(const uint8_t frameIdx) const;

		inline QUEUE_FLAGS getQueueFlags() const ;

		inline uint8_t getQueueFamily() const;
		
		inline VkQueue getVkQueue() const;

		inline ValProc* getValProc() const;

		static uint8_t findQueueFamilyOfQueueFlags(QUEUE_FLAGS flag, ValProc* proc, VkSurfaceKHR surface/*optional*/, bool* success/*optional*/);

		void create(ValProc& proc, const QUEUE_FLAGS flags);

		void create(ValProc& proc, const uint8_t cmdBuffAndSemaphoreCount, const QUEUE_FLAGS flags);

		// release allocated objects
		void destroy();
	protected:
		friend Window;
		friend ValProc;

		void create();

		void setCmdBuffAndSemaphoreCountFromProc(ValProc& proc);

		void copyToOther(Queue* other) const;

	#ifndef NDEBUG
		void dbgValidateSelfUse() const;
	#endif // !NDEBUG

		// returns true if successful
		bool findQueueFamily(uint8_t* queueFamilyOut,VkSurfaceKHR surface = NULL);

	private:
		VkQueue _vkQueue = NULL;
		ValProc* _proc = NULL;

		VkCommandBuffer* _commandBuffers = NULL;
		VkSemaphore* _semaphores = NULL;

		QUEUE_FLAGS _queueFlags = QUEUE_FLAGS::Graphics;

		uint8_t _queueFamily = 0;
		uint8_t _cmdBuffAndSemaphoreBufferCount = 0u;
	};
}

#endif // !VAL_QUEUE_HPP