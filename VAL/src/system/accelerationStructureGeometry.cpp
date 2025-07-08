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

#include <VAL/lib/system/accelerationStructureGeometry.hpp>
#include <VAL/lib/system/system_utils.hpp>

namespace val
{
	void AccelerationStructureGeometry::create()
	{

	}
	
	void AccelerationStructureGeometry::setTriangleGeometryData(VkDevice device, const VkDeviceSize vertexStride, const VkFormat positionFormat, const VkBuffer vertexBuffer, const uint32_t indexCount)
	{
		geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;

		geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
		geometry.triangles.pNext = VK_NULL_HANDLE;
		geometry.triangles.vertexFormat = positionFormat;
		const VkBufferDeviceAddressInfo vertexBuffAddressInfo =
		{
			.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
			.pNext = VK_NULL_HANDLE,
			.buffer = vertexBuffer
		};
		geometry.triangles.vertexData = (VkDeviceOrHostAddressConstKHR)vkGetBufferDeviceAddress(device, &vertexBuffAddressInfo);
		geometry.triangles.vertexStride = vertexStride;
		geometry.triangles.maxVertex = indexCount;
		geometry.triangles.indexType = VK_INDEX_TYPE_NONE_KHR;
		geometry.triangles.indexData = { NULL };
		//geometry.triangles.transformData = (VkDeviceOrHostAddressConstKHR)vkGetBufferDeviceAddress(device, &indexBuffAddressInfo);
	}

	void AccelerationStructureGeometry::setTriangleGeometryData(VkDevice device, const VkDeviceSize vertexStride, const VkFormat positionFormat, const VkBuffer vertexBuffer, const VkBuffer indexBuffer, const uint32_t vertexCount)
	{
		geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
		geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
		geometry.triangles.pNext = VK_NULL_HANDLE;
		geometry.triangles.vertexFormat = positionFormat;
#ifndef NDEBUG
		if (!vertexBuffer) {
			dbg::printError("AccelerationStructureGeometry::setTriangleGeometryData: vertexBuffer is NULL, cannot set geometry data.");
			throw std::runtime_error("AccelerationStructureGeometry::setTriangleGeometryData: vertexBuffer is NULL, cannot set geometry data.");
		}
#endif // !NDEBUG

		const VkBufferDeviceAddressInfo vertexBuffAddressInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
			.buffer = vertexBuffer
		};

		geometry.triangles.vertexData.deviceAddress = vkGetBufferDeviceAddress(device, &vertexBuffAddressInfo);
		geometry.triangles.vertexStride = vertexStride;
		geometry.triangles.maxVertex = vertexCount - 1; // maxVertex is the number of vertices in vertexData minus one.

		geometry.triangles.indexType = indexBuffer ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_NONE_KHR;

		if (indexBuffer) {
			const VkBufferDeviceAddressInfo indexBuffAddressInfo = {
				.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
				.buffer = indexBuffer
			};
			geometry.triangles.indexData.deviceAddress = vkGetBufferDeviceAddress(device, &indexBuffAddressInfo);
		}
		else {
			geometry.triangles.indexData.deviceAddress = NULL;
		}
		geometry.triangles.transformData = { NULL };
		//geometry.triangles.transformData = (VkDeviceOrHostAddressConstKHR)vkGetBufferDeviceAddress(device, &indexBuffAddressInfo);
	}

	void AccelerationStructureGeometry::setAabbGeometryData()
	{

	}

	void AccelerationStructureGeometry::setInstancesGeometryData()
	{

	}
}