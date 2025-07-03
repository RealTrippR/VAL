#ifndef VAL_ACCELERATION_STRUCTURE_INSTANCE_HPP
#define VAL_ACCELERATION_STRUCTURE_INSTANCE_HPP

#include <vulkan/vulkan_core.h>
namespace val
{
	class AccelerationStructure; // forward declaration

	class AccelerationStructureInstance
	{
		//void create(AccelerationStructure& accelStruct);

		inline void setTransform(const VkTransformMatrixKHR transform);

		inline VkTransformMatrixKHR getTransform() const;

		inline void setFlags(const VkGeometryInstanceFlagsKHR flags);

		inline VkGeometryInstanceFlagsKHR getFlags() const;

		inline void setShaderBindingRecordTableOffset(const uint32_t offset);

		inline uint32_t getShaderBindingRecordTableOffset() const;

		inline VkAccelerationStructureInstanceKHR& asVkAccelerationStructureInstanceKHR();
		/*
		   uint32_t                      instanceCustomIndex:24;
    uint32_t                      mask:8;
    uint32_t                      instanceShaderBindingTableRecordOffset:24;
    VkGeometryInstanceFlagsKHR    flags:8;
    uint64_t                      accelerationStructureReference;
	*/
	private:
		/*	VkAccelerationStructureDeviceAddressInfoKHR addressInfo{};
		addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
		addressInfo.accelerationStructure = yourBlasHandle;

		VkDeviceAddress blasAddress = vkGetAccelerationStructureDeviceAddressKHR(device, &addressInfo);
		*/
		VkAccelerationStructureInstanceKHR accelStructInstance;
	};




	inline void AccelerationStructureInstance::setTransform(const VkTransformMatrixKHR transform)
	{
		accelStructInstance.transform = transform;
	}

	inline VkTransformMatrixKHR AccelerationStructureInstance::getTransform() const
	{
		return accelStructInstance.transform;
	}

	inline void AccelerationStructureInstance::setFlags(const VkGeometryInstanceFlagsKHR flags)
	{
		accelStructInstance.flags = flags;
	}

	inline VkGeometryInstanceFlagsKHR AccelerationStructureInstance::getFlags() const
	{
		return accelStructInstance.flags;
	}

	inline void AccelerationStructureInstance::setShaderBindingRecordTableOffset(const uint32_t offset)
	{
		accelStructInstance.instanceShaderBindingTableRecordOffset = offset;
	}

	inline uint32_t AccelerationStructureInstance::getShaderBindingRecordTableOffset() const
	{
		return accelStructInstance.instanceShaderBindingTableRecordOffset;
	}

	inline VkAccelerationStructureInstanceKHR& AccelerationStructureInstance::asVkAccelerationStructureInstanceKHR() 
	{
		return accelStructInstance;
	}
}

#endif // !VAL_ACCELERATION_STRUCTURE_INSTANCE_HPP