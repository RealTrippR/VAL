#include <VAL/lib/system/shaderBindingTable.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/raytracingPipeline.hpp>

namespace val
{

	uint32_t alignUp(uint32_t val, uint32_t alignment) {
		return (val + alignment - 1) & ~(alignment - 1);
	}


	VAL_RETURN_CODE ShaderBindingTable::createForRaytracingPipeline(ValProc& proc, RaytracingPipeline& pipeline)
	{
		PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR =
			(PFN_vkGetRayTracingShaderGroupHandlesKHR)vkGetDeviceProcAddr(proc.getVkLogicalDevice(), "vkGetRayTracingShaderGroupHandlesKHR");

		// get physical device raytracing capabilities
		VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtProps = {};
		rtProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;

		VkPhysicalDeviceProperties2 props2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
		props2.pNext = &rtProps;
		vkGetPhysicalDeviceProperties2(proc.getVkPhysicalDevice(), &props2);


		uint32_t handleSize = rtProps.shaderGroupHandleSize;
		uint32_t handleAlignment = rtProps.shaderGroupBaseAlignment;

		const uint32_t groupCount = pipeline.getRaygroups().size();
		const uint32_t alignedHandleSize = alignUp(handleSize, handleAlignment);


		// allocate sbt
		const VkDeviceSize sbtSize = groupCount * alignedHandleSize;
		if (sbtSize > UINT32_MAX) {
			dbg::printError("ShaderBindingTable::createForRaytracingPipeline: Failed to allocate Shader Binding Table buffer for ShaderBindingTable @ %p ShaderBindingTable: size exceeds UINT32_MAX.", true);
			return VAL_FAILURE;
		}

		shaderHandleStorage.resize((uint32_t)sbtSize);
		vkGetRayTracingShaderGroupHandlesKHR(proc.getVkLogicalDevice(), proc._raytracingPipelines[0], 0, groupCount, sbtSize, shaderHandleStorage.data());

		proc.createBuffer(sbtSize, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, _sbtBuffer, _sbtMemory);


		// Map and copy aligned shader handles
		uint8_t* mapped;
		vkMapMemory(proc.getVkLogicalDevice(), _sbtMemory, 0, handleSize, 0x0, (void**)&mapped); // map sbtMemory
		for (uint32_t i = 0; i < groupCount; ++i) {
			memcpy(mapped + i * alignedHandleSize, shaderHandleStorage.data() + i * handleSize, handleSize);
		}








		VkBufferDeviceAddressInfo sbtAddressInfo;
		sbtAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		sbtAddressInfo.pNext = NULL;
		sbtAddressInfo.buffer = _sbtBuffer;

		VkDeviceAddress sbtAddress = vkGetBufferDeviceAddress(proc.getVkLogicalDevice(), &sbtAddressInfo);

		uint32_t raygenHandleCount = 0u;
		uint32_t missHandleCount = 0u;
		uint32_t hitHandleCount=0u;
		uint32_t callableHandleCount = 0u;
		uint32_t intersectionShadersCount = 0;

		for (uint32_t rgi = 0; rgi < pipeline.getRaygroups().size(); ++rgi)
		{
			RaytracingGroup& raygroup = pipeline.getRaygroups()[rgi];

			for (uint32_t si = 0; si < raygroup.shaders.size(); ++si)
			{
				Shader* shdr = raygroup.shaders[si];
				if (!shdr) {
					continue;
				}
				else {
					if (shdr->getStageFlags() == SHADER_STAGE::Raygen) {
						raygenHandleCount++;
					}
					if (shdr->getStageFlags() == SHADER_STAGE::Miss) {
						missHandleCount++;
					}
					if (shdr->getStageFlags() == SHADER_STAGE::Callable) {
						callableHandleCount++;
					}
					if (shdr->getStageFlags() == SHADER_STAGE::AnyHit) {
						hitHandleCount++;
					}
					if (shdr->getStageFlags() == SHADER_STAGE::ClosestHit) {
						hitHandleCount++;
					}
					if (shdr->getStageFlags() == SHADER_STAGE::Intersection) {
						hitHandleCount++;
					}
				}
			}
		}
		
		uint32_t curHandleCount = 0u;
		// RAYGEN
		_raygenRegion = {
			.deviceAddress = sbtAddress + 0 * alignedHandleSize,
			.stride = alignedHandleSize,
			.size = alignedHandleSize * raygenHandleCount
		};
		curHandleCount += 1;
		
		// MISS
		_missRegion = {
			.deviceAddress = sbtAddress + curHandleCount * alignedHandleSize,
			.stride = alignedHandleSize,
			.size = alignedHandleSize * missHandleCount
		};
		curHandleCount += missHandleCount;


		// HIT
		_hitRegion = {
			.deviceAddress = sbtAddress + curHandleCount * alignedHandleSize,
			.stride = alignedHandleSize,
			.size = hitHandleCount * alignedHandleSize
		};
		curHandleCount += hitHandleCount;

		// CALLABLE
		_callableRegion = {
			.deviceAddress = sbtAddress + curHandleCount * alignedHandleSize,
			.stride = alignedHandleSize,
			.size = callableHandleCount * alignedHandleSize
		};
		curHandleCount += callableHandleCount;


		return VAL_SUCCESS;
	}
}