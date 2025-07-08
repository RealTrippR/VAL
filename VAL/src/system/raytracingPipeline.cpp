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

	void RaytracingPipeline::setRaygroups(const tiny_vector<RaytracingGroup>& raygroups)
	{
		_raygroups = raygroups;
#ifndef NDEBUG
		for (uint32_t i = 0; i < raygroups.size(); ++i)
		{
			raygroups[i].dbgValidateRaygroup();
		}
#endif // !NDEBUG
	}

	const tiny_vector<RaytracingGroup>& RaytracingPipeline::getRaygroups() const
	{
		return _raygroups;
	}

	tiny_vector<RaytracingGroup>& RaytracingPipeline::getRaygroups()
	{
		return _raygroups;
	}

}