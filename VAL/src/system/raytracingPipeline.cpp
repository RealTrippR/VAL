#include <VAL/lib/system/raytracingPipeline.hpp>

namespace val
{
	void RaytracingPipeline::setMaxRayRecursionDepth(const uint32_t depth)
	{
		_maxRayRecursionDepth = depth;
	}

	uint32_t RaytracingPipeline::getMaxRayRecursionDepth() const
	{
		return _maxRayRecursionDepth;
	}
}