#include <VAL/lib/system/rayTracingGroup.hpp>
#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/graphics/shader.hpp>

namespace val
{
	void RaytracingGroup::dbgValidateRaygroup() const
	{
		uint32_t raygenCount = 0u;
		uint32_t missCount = 0u;
		uint32_t callableCount = 0u;
		uint32_t anyhitCount = 0u;
		uint32_t closestHitCount = 0u;
		uint32_t intersection = 0u;

		for (Shader* shdr : shaders)
		{
			if (shdr->getStageFlags() == SHADER_STAGE::Raygen)
			{
				raygenCount++;
			}
			if (shdr->getStageFlags() == SHADER_STAGE::Miss)
			{
				missCount++;
			}
			if (shdr->getStageFlags() == SHADER_STAGE::Callable)
			{
				callableCount++;
			}

			if (shdr->getStageFlags() == SHADER_STAGE::ClosestHit)
			{
				closestHitCount++;
			}


			if (shdr->getStageFlags() == SHADER_STAGE::AnyHit)
			{
				anyhitCount++;
			}


			if (shdr->getStageFlags() == SHADER_STAGE::Intersection)
			{
				intersection++;
			}
		}










		if (raygenCount > 1) {
			dbg::printError("RaytracingGroup:dbgValidateRaygroup: RaytracingGroup @ %p has more than 1 raygen shader, this is invalid.");
		}
		if (anyhitCount > 1) {
			dbg::printError("RaytracingGroup:dbgValidateRaygroup: RaytracingGroup @ %p has more than 1 any-hit shader, this is invalid.");
		}
		if (closestHitCount > 1) {
			dbg::printError("RaytracingGroup:dbgValidateRaygroup: RaytracingGroup @ %p has more than 1 closest hit shader, this is invalid.");
		}
		if (intersection > 1) {
			dbg::printError("RaytracingGroup:dbgValidateRaygroup: RaytracingGroup @ %p has more than 1 intersection shader, this is invalid.");
		}
	}
}