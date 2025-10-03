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

#include <VAL/lib/system/accelerationStructure.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/buffer.hpp>
#include <VAL/lib/ext/roundToNextPowerOf2.hpp>
#include <unordered_map>

namespace val
{

	void AccelerationStructure::setInstances(const tiny_vector<AccelerationStructureInstance*>& instances)
	{
		_instances = instances;
	}


	const tiny_vector<AccelerationStructureInstance*>& AccelerationStructure::getInstances() const
	{
		return _instances;
	}

	tiny_vector<AccelerationStructureInstance*>& AccelerationStructure::getInstances()
	{
		return _instances;
	}


	tiny_vector<const AccelerationStructureGeometry*> AccelerationStructure::getGeometries() const
	{
		std::set<const AccelerationStructureGeometry*> geometries;
		
		for (AccelerationStructureInstance* inst : _instances) {
			geometries.insert(inst->getGeometry());
		}

		tiny_vector<const AccelerationStructureGeometry*> v;

		for (const AccelerationStructureGeometry* geom : geometries) {
			v.push_back(geom);
		}

		return v;
	}

	void AccelerationStructure::setAccelerationStructureInstances(const tiny_vector<AccelerationStructureInstance*>& instances)
	{
		_instances = instances;
	}

	const tiny_vector<AccelerationStructureInstance*>& AccelerationStructure::getAccelerationStructureInstances() const
	{
		return _instances;
	}

	VAL_RETURN_CODE AccelerationStructure::buildAsBottomLevel(Queue& rayqueue, uint32_t vertexCount, uint32_t indexCount, VkCommandBuffer cmdBuffer)
	{
		const VkBuildAccelerationStructureFlagsKHR ACCEL_BUILD_FLAGS = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		const VkAccelerationStructureTypeKHR ACCEL_STRUCT_TYPE = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

		auto& proc = *rayqueue.getValProc();
		PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR = 
			(PFN_vkCmdBuildAccelerationStructuresKHR)vkGetDeviceProcAddr(proc.getVkLogicalDevice(), "vkCmdBuildAccelerationStructuresKHR");

		PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR =
			(PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(proc.getVkLogicalDevice(), "vkCreateAccelerationStructureKHR");
		
		tiny_vector<const AccelerationStructureGeometry*> geometries(_instances.size());

		for (uint32_t i = 0; i < _instances.size(); ++i)
		{
			geometries[i] = _instances[i]->getGeometry();
			if (geometries[i] == NULL) {
				dbg::printError("AccelerationStructure::buildAsBottomLevel: Cannot build, instance #%lu has NULL geometry.", i);
				return VAL_FAILURE;
			}
		}


		if (_instances.size() == 0) {
			dbg::printWarning("AccelerationStructure::buildAsBottomLevel: AccelerationStructure @ %p has no _instances, undefined behavior may occur.", this);
		}


		/*per the Vulkan 1.4.3 spec: */
		/*pMaxPrimitiveCounts is a pointer to an array of pBuildInfo->geometryCount uint32_t values defining the number of primitives built into each geometry.*/
		tiny_vector<uint32_t> primitiveCounts(geometries.size());
		tiny_vector<VkAccelerationStructureGeometryKHR> geometriesAsLinearMemory(geometries.size());

		for (uint32_t i = 0; i < geometries.size(); ++i)
		{
			if (indexCount>0) {
				primitiveCounts[i] = indexCount / 3;
			}
			else {
				primitiveCounts[i] = vertexCount / 3;
			}
			
			geometriesAsLinearMemory[i] = *(VkAccelerationStructureGeometryKHR*)(geometries[i]);

			geometriesAsLinearMemory[i].geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
			geometriesAsLinearMemory[i].flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
		}
		
		// https://registry.khronos.org/vulkan/specs/latest/man/html/VkAccelerationStructureBuildGeometryInfoKHR.html
		VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};
		buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		buildInfo.flags = ACCEL_BUILD_FLAGS;
		buildInfo.geometryCount = geometriesAsLinearMemory.size();
		buildInfo.pGeometries = geometriesAsLinearMemory.data();
		buildInfo.ppGeometries = VK_NULL_HANDLE;
		buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		buildInfo.srcAccelerationStructure = VK_NULL_HANDLE; /*srcAccelerationStructure is a pointer to an existing 
															acceleration structure that is to be used to update the 
															dstAccelerationStructure acceleration structure when mode
															is VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR.*/

		VkAccelerationStructureBuildSizesInfoKHR sizeInfo = getBuildSizes(proc, &buildInfo, primitiveCounts.data());


		// CREATE SCRATCH BUFFER
		proc.createBuffer(sizeInfo.buildScratchSize, 
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
			_transientScratchBuffer, _transientScratchMemory);


		// CREATE ACCELERATION STRUCTURE BUFFER
		proc.createBuffer(sizeInfo.accelerationStructureSize,
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
			_accelStructBuffer, _accelStructMemory);


		// https://registry.khronos.org/vulkan/specs/latest/man/html/VkAccelerationStructureCreateInfoKHR.html
		const VkAccelerationStructureCreateInfoKHR AccelStructCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR,
			.pNext = VK_NULL_HANDLE,
			.createFlags = 0x0,
			.buffer = _accelStructBuffer, // must be a buffer created with usage flag VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR
			.offset = 0x0,
			.size = sizeInfo.accelerationStructureSize,
			.type = ACCEL_STRUCT_TYPE,
			.deviceAddress = NULL //  If deviceAddress is zero, no specific address is requested.
		};


		// update build info with scratch buffer address
		const VkBufferDeviceAddressInfo scratchBuffAddressInfo =
		{
			.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
			.buffer = _transientScratchBuffer
		};
		buildInfo.scratchData.deviceAddress = vkGetBufferDeviceAddress(proc.getVkLogicalDevice(), &scratchBuffAddressInfo);

		if (vkCreateAccelerationStructureKHR(proc.getVkLogicalDevice(), &AccelStructCreateInfo, nullptr, &_accelStructure) != VK_SUCCESS) {
			dbg::printError("AccelerationStructure::buildAsBottomLevel: Failed to create acceleration structure for AccelerationStructure %p", this);
			return VAL_FAILURE;
		}


		// update accel struct to build to, now that it has been created
		buildInfo.dstAccelerationStructure = _accelStructure;


		// create build ranges
		const uint32_t infoCount = 1u;
		VkAccelerationStructureBuildRangeInfoKHR** buildRanges;
		buildRanges = (VkAccelerationStructureBuildRangeInfoKHR**)malloc(infoCount * sizeof(VkAccelerationStructureBuildRangeInfoKHR*));
		if (buildRanges == VK_NULL_HANDLE)
		{
			dbg::printError("AccelerationStructure::buildAsBottomLevel: Failed to allocate build ranges for AccelerationStructure @ %p.",this);
			return VAL_FAILURE;
		}

		for (uint32_t i = 0; i < infoCount; ++i)
		{
			buildRanges[i] = (VkAccelerationStructureBuildRangeInfoKHR*)malloc(buildInfo.geometryCount * sizeof(VkAccelerationStructureBuildRangeInfoKHR));
			// https://registry.khronos.org/vulkan/specs/latest/man/html/VkAccelerationStructureBuildRangeInfoKHR.html
			auto& buildRange = buildRanges[i];
			if (buildRanges[i] != VK_NULL_HANDLE)
			{
				buildRange->primitiveCount = primitiveCounts[i];
				buildRange->primitiveOffset = 0u;
				buildRange->transformOffset = 0u;
				buildRange->firstVertex = 0u;
			}
			else {
				dbg::printWarning("AccelerationStructure::buildAsBottomLevel: buildRanges[%lu] is NULL.", i);
			}
		}

		VkFence fence;
		VkFenceCreateInfo fenceCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0x0
		};


		//vkCreateFence(proc, &fenceCreateInfo, NULL, &fence);

		const VkShaderStageFlags stageFlags = VK_SHADER_STAGE_ALL;

		vkCmdBuildAccelerationStructuresKHR(
			cmdBuffer,
			infoCount,
			&buildInfo,
			buildRanges);

		// wait for acceleration structure to build (silent errors may occur otherwise)
		//vkWaitForFences(proc, 1, &fence, VK_TRUE, UINT64_MAX);

		//vkDestroyFence(proc, fence, NULL);

		// destroy build ranges
		for (uint32_t i = 0; i < infoCount; ++i)
		{
			if (buildRanges[i])
			{
				free(buildRanges[i]);
			}
		}
		free(buildRanges);

		return VAL_SUCCESS;
	}

	VAL_RETURN_CODE AccelerationStructure::buildAsTopLevel(Queue& q, AccelerationStructure& BLAS, Queue& rayqueue, VkCommandBuffer cmdBuffer)
	{
		const VkBuildAccelerationStructureFlagsKHR ACCEL_BUILD_FLAGS = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		const VkAccelerationStructureTypeKHR ACCEL_STRUCT_TYPE = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;

		auto& proc = *rayqueue.getValProc();

		PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR =
			(PFN_vkGetAccelerationStructureBuildSizesKHR)vkGetDeviceProcAddr(proc.getVkLogicalDevice(), "vkGetAccelerationStructureBuildSizesKHR");

		PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR =
			(PFN_vkCmdBuildAccelerationStructuresKHR)vkGetDeviceProcAddr(proc.getVkLogicalDevice(), "vkCmdBuildAccelerationStructuresKHR");

		PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR =
			(PFN_vkCreateAccelerationStructureKHR)vkGetDeviceProcAddr(proc.getVkLogicalDevice(), "vkCreateAccelerationStructureKHR");

		PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR =
			(PFN_vkGetAccelerationStructureDeviceAddressKHR)vkGetDeviceProcAddr(proc.getVkLogicalDevice(), "vkGetAccelerationStructureDeviceAddressKHR");






		tiny_vector<AccelerationStructureInstance*>& instancesVAL = BLAS.getInstances();
		const uint32_t AS_instancesCount = instancesVAL.size();

		tiny_vector<VkAccelerationStructureInstanceKHR> instancesVK(AS_instancesCount);


		VkAccelerationStructureBuildGeometryInfoKHR instanceBuildInfo = {};

		VkDeviceOrHostAddressConstKHR instanceBufferDeviceAddress = {};
		// create instance and instance buffer
		{
			// get BLAS device address
			const VkAccelerationStructureDeviceAddressInfoKHR BLAS_addrInfo =
			{
				.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR,
				.pNext = VK_NULL_HANDLE,
				.accelerationStructure = BLAS._accelStructure
			};
			const VkDeviceAddress BLAS_acstAddress = vkGetAccelerationStructureDeviceAddressKHR(proc.getVkLogicalDevice(), &BLAS_addrInfo);



			for (uint32_t i = 0; i < instancesVK.size(); ++i)
			{
				VkAccelerationStructureInstanceKHR& instance = instancesVK[i];
				instance = instancesVAL[i]->asVkAccelerationStructureInstanceKHR();

				instance.accelerationStructureReference = BLAS_acstAddress;
				instance.instanceCustomIndex = 0;
				instance.mask = 0xFF;
				instance.instanceShaderBindingTableRecordOffset = 0u;
			}
			// create acceleration structure instance(s)
			//https://registry.khronos.org/vulkan/specs/latest/man/html/VkAccelerationStructureInstanceKHR.html
			//VkAccelerationStructureInstanceKHR AS_Instance{};
			//AS_Instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR | VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_KHR;
			//AS_Instance.accelerationStructureReference = BLAS_acstAddress;
			//AS_Instance.instanceCustomIndex = 0;
			//AS_Instance.mask = 0xFF;
			//AS_Instance.instanceShaderBindingTableRecordOffset = 0;
			///* The transform defines the transformation of an instance of bottom level geometry.
			//* it includes rotation, scale, shear, and position data/
			//| R11 R12 R13 X |
			//| R21 R22 R23 Y |
			//| R31 R32 R33 Z |
			//*/
			//const float X = 0, Y = 0, Z = 0;
			//AS_Instance.transform = {
			//	1, 0, 0, X,
			//	0, 1, 0, Y,
			//	0, 0, 1, Z
			//};


			const uint32_t instanceBuffSize = sizeof(VkAccelerationStructureInstanceKHR) * instancesVK.size();

			proc.createBuffer(instanceBuffSize,
				VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR
				| VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT,
				_instanceBuff, _instanceBuffMem);


			// instance buff is GPU local, 
			// so we have to create a staging buffer and copy the data from a staging buffer into the instance buffer.
			VkBuffer stagingBuff;
			VkDeviceMemory stagingBuffMem;
			char* mappedDataStagingBuff;
			proc.createBuffer(sizeof(VkAccelerationStructureInstanceKHR),
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				bufferSpaceToVkMemoryProperty(BUFFER_SPACE::CPU_GPU),
				stagingBuff, stagingBuffMem);

			vkMapMemory(proc._device, stagingBuffMem, 0, sizeof(VkAccelerationStructureInstanceKHR),
				0x0, (void**)&mappedDataStagingBuff);

			memcpy_s(mappedDataStagingBuff, instanceBuffSize, instancesVK.data(), instanceBuffSize);
			proc.copyBuffer(q, stagingBuff, _instanceBuff, instanceBuffSize, 0, 0);

			vkUnmapMemory(proc._device, stagingBuffMem);
			vkFreeMemory(proc._device, stagingBuffMem, NULL);
			vkDestroyBuffer(proc._device, stagingBuff, NULL);

			// Get device address of instance buffer
			VkBufferDeviceAddressInfo addressInfo = {
				.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
				.buffer = _instanceBuff
			};
			instanceBufferDeviceAddress.deviceAddress = vkGetBufferDeviceAddress(proc.getVkLogicalDevice(), &addressInfo);
		}

		VkAccelerationStructureGeometryKHR geometry{};
		geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		geometry.pNext = VK_NULL_HANDLE;
		geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
		geometry.flags = 0x0; // must be either 0 or VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR
		geometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
		geometry.geometry.instances.pNext = VK_NULL_HANDLE;
		geometry.geometry.instances.arrayOfPointers = VK_FALSE;
		geometry.geometry.instances.data = instanceBufferDeviceAddress;

		instanceBuildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		instanceBuildInfo.pNext = VK_NULL_HANDLE;
		instanceBuildInfo.flags = ACCEL_BUILD_FLAGS;
		instanceBuildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		instanceBuildInfo.type = ACCEL_STRUCT_TYPE;
		instanceBuildInfo.flags = ACCEL_BUILD_FLAGS;
		instanceBuildInfo.geometryCount = 1;
		instanceBuildInfo.pGeometries = &geometry;

		VkAccelerationStructureBuildSizesInfoKHR sizeInfo = getBuildSizes(proc, &instanceBuildInfo, &AS_instancesCount);

		// CREATE SCRATCH BUFFER
		proc.createBuffer(sizeInfo.buildScratchSize,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, _transientScratchBuffer, _transientScratchMemory);


		// CREATE ACCELERATION STRUCTURE BUFFER
		proc.createBuffer(sizeInfo.accelerationStructureSize,
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, _accelStructBuffer, _accelStructMemory);





		VkAccelerationStructureCreateInfoKHR AccelStructCreateInfo{};
		AccelStructCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		AccelStructCreateInfo.pNext = VK_NULL_HANDLE;
		AccelStructCreateInfo.buffer = _accelStructBuffer;
		AccelStructCreateInfo.size = sizeInfo.accelerationStructureSize;
		AccelStructCreateInfo.type = ACCEL_STRUCT_TYPE;

		if (vkCreateAccelerationStructureKHR(proc.getVkLogicalDevice(), &AccelStructCreateInfo, nullptr, &_accelStructure) != VK_SUCCESS) {
			dbg::printError("AccelerationStructure::buildAsBottomLevel: Failed to create acceleration structure for AccelerationStructure %p", this);
			return VAL_FAILURE;
		}

		// get scratch buffer address
		const VkBufferDeviceAddressInfo scratchBuffAddressInfo =
		{
			.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
			.pNext = VK_NULL_HANDLE,
			.buffer = _transientScratchBuffer
		};
		instanceBuildInfo.scratchData.deviceAddress = vkGetBufferDeviceAddress(proc.getVkLogicalDevice(), &scratchBuffAddressInfo);
		// set accel structure to build to
		instanceBuildInfo.dstAccelerationStructure = _accelStructure;

		VkAccelerationStructureBuildRangeInfoKHR buildRange{};
		buildRange.primitiveCount = 1u; // 1 for 1 instance (VkAccelerationStructureGeometryKHR)

		const VkAccelerationStructureBuildRangeInfoKHR* pRanges = &buildRange;

		/*VkFence fence;
		VkFenceCreateInfo fenceCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0x0
		};*/


		//vkCreateFence(proc, &fenceCreateInfo, NULL, &fence);

		const VkShaderStageFlags stageFlags = VK_SHADER_STAGE_ALL;

		vkCmdBuildAccelerationStructuresKHR(
			cmdBuffer,
			1,
			&instanceBuildInfo,
			&pRanges);

	
		// wait for acceleration structure to build (silent errors may occur otherwise)
		//vkWaitForFences(proc, 1, &fence, VK_TRUE, UINT64_MAX);

		//vkDestroyFence(proc, fence, NULL);

		return VAL_SUCCESS;
	}


	uint64_t roundUp(uint64_t size, uint64_t multiple)
	{
		const uint64_t u = (size + multiple-1) / multiple;
		return u * multiple;
	}

	VkAccelerationStructureBuildSizesInfoKHR AccelerationStructure::getBuildSizes(ValProc& proc, const VkAccelerationStructureBuildGeometryInfoKHR* pBuildInfo, const uint32_t* pMaxPrimitiveCounts) const
	{

		PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR =
			(PFN_vkGetAccelerationStructureBuildSizesKHR)vkGetDeviceProcAddr(proc, "vkGetAccelerationStructureBuildSizesKHR");

		VkPhysicalDeviceAccelerationStructurePropertiesKHR accelProps = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR
		};


		VkPhysicalDeviceProperties2 props2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
		props2.pNext = &accelProps;
		vkGetPhysicalDeviceProperties2(proc.getVkPhysicalDevice(), &props2);

		VkAccelerationStructureBuildSizesInfoKHR sizeInfo = {};
		sizeInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

		vkGetAccelerationStructureBuildSizesKHR(
			proc.getVkLogicalDevice(),
			VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
			pBuildInfo,
			pMaxPrimitiveCounts,
			&sizeInfo);

		// AccelerationStructure offset needs to be 256 bytes aligned, as per vulkan spec
		const uint64_t AccelerationStructureAlignment = 256;
		const uint64_t ScratchAlignment = accelProps.minAccelerationStructureScratchOffsetAlignment;

		sizeInfo.accelerationStructureSize = roundUp(sizeInfo.accelerationStructureSize, AccelerationStructureAlignment);
		sizeInfo.buildScratchSize = roundUp(sizeInfo.buildScratchSize, ScratchAlignment);

		return sizeInfo;
	}



	/*

	void AccelerationStructure::build(ValProc& proc, AccelerationStructure& TLAS, Queue& rayqueue)
	{
		tiny_vector< VkAccelerationStructureBuildRangeInfoKHR> buildRanges;

		VkDeviceOrHostAddressConstKHR instanceDataDeviceAddress = {};
		{
			proc.createBuffer(sizeof(VkAccelerationStructureInstanceKHR),
				VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR
				| VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
				bufferSpaceToVkMemoryProperty(bufferSpace::GPU_ONLY),
				_instanceBuff, _instanceBuffMem);

			// Get device address of instance buffer
			VkBufferDeviceAddressInfo addressInfo = {
				.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
				.pNext = VK_NULL_HANDLE,
				.buffer = _instanceBuff
			};

			instanceDataDeviceAddress.deviceAddress = vkGetBufferDeviceAddress(proc.getVkLogicalDevice(), &addressInfo);
		}

		VkAccelerationStructureDeviceAddressInfoKHR addressInfo{};
		addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
		addressInfo.accelerationStructure = _accelStructure;

		VkAccelerationStructureGeometryKHR geometry{};
		geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
		geometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
		geometry.geometry.instances.arrayOfPointers = VK_FALSE;
		geometry.geometry.instances.data = instanceDataDeviceAddress;
		geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;

		VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};
		buildInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
		buildInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		buildInfo.geometryCount = 1;
		buildInfo.pGeometries = &geometry;
		buildInfo.srcAccelerationStructure = VK_NULL_HANDLE;
		buildInfo.dstAccelerationStructure = yourTlasHandle;  // set this to your TLAS handle
		buildInfo.scratchData.deviceAddress = scratchBuffer.deviceAddress;


		VkAccelerationStructureBuildRangeInfoKHR buildRange{};
		buildRange.primitiveCount = 1;
		buildRange.primitiveOffset = 0;
		buildRange.firstVertex = 0;
		buildRange.transformOffset = 0;

		const VkAccelerationStructureBuildRangeInfoKHR* pBuildRanges = &buildRange;

		vkCmdBuildAccelerationStructuresKHR(
			rayqueue.getCommandBuffer(0),
			1,
			&buildInfo,
			&pBuildRanges
		);
	}
	*/
}