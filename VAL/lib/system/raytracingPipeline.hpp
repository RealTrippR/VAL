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

#ifndef VAL_RAYTRACING_PIPELINE_HPP
#define VAL_RAYTRACING_PIPELINE_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/pipelineBase.hpp>

#include <VAL/lib/system/accelerationStructureGeometry.hpp>
#include <VAL/lib/system/accelerationStructureInstance.hpp>
#include <VAL/lib/system/accelerationStructure.hpp>

namespace val
{
	class RaytracingPipeline : public PipelineBase
	{
	public:
		RaytracingPipeline() {
			_bindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
		}

		void setMaxRayRecursionDepth(const uint32_t depth);

		uint32_t getMaxRayRecursionDepth() const;

	private:
		uint32_t _maxRayRecursionDepth;
	};
}

#endif // !VAL_RAYTRACING_PIPELINE_HPP