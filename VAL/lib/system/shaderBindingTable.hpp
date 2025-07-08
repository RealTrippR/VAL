#ifndef VAL_SHADER_BINDING_TABLE_HPP
#define VAL_SHADER_BINDING_TABLE_HPP

#include <VAL/lib/system/system_utils.hpp>

namespace val
{
	class RaytracingPipeline; // Forward declaration
	class ShaderBindingTable
	{
	public:
		VAL_RETURN_CODE createForRaytracingPipeline(ValProc& proc, RaytracingPipeline& pipeline);

		inline VkStridedDeviceAddressRegionKHR& getRaygenRegion() { return _raygenRegion; }

		inline VkStridedDeviceAddressRegionKHR& getMissRegion() { return _missRegion; }

		inline VkStridedDeviceAddressRegionKHR& getHitRegion() {return _hitRegion;}

		inline VkStridedDeviceAddressRegionKHR& getCallableRegion() { return _callableRegion; }

	protected:

		VkStridedDeviceAddressRegionKHR _raygenRegion = {};
		VkStridedDeviceAddressRegionKHR _missRegion = {};
		VkStridedDeviceAddressRegionKHR _hitRegion = {};
		VkStridedDeviceAddressRegionKHR _callableRegion = {};

		uint8_t* _mappedSbtMemory = VK_NULL_HANDLE;
		VkBuffer _sbtBuffer=VK_NULL_HANDLE;
		VkDeviceMemory _sbtMemory = VK_NULL_HANDLE;

		tiny_vector<uint8_t> shaderHandleStorage;
	};
}



#endif // !VAL_SHADER_BINDING_TABLE_HPP