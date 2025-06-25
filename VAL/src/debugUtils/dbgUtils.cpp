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

#include <VAL/lib/debugUtils/dbgUtils.hpp>
#include <VAL/lib/debugReporting/debugCallbacks.hpp>

#include <set>
#include <unordered_map>
namespace val
{
	struct debugFunctions
	{
		PFN_vkDebugMarkerSetObjectNameEXT setObjectName;
		PFN_vkDebugMarkerSetObjectTagEXT setObjectTag;
		PFN_vkCmdDebugMarkerInsertEXT insertMarker;
		PFN_vkCmdDebugMarkerBeginEXT beginMarker;
		PFN_vkCmdDebugMarkerEndEXT endMarker;
	};

	std::set<void*> objBreakOnCreateList;
	std::set<void*> objBreakOnDestroyList;

	struct dbgInfoForDevice {
		std::set<void*> allocatedObjects;

		struct debugFunctions functions;
	};

	std::unordered_map<VkDevice, dbgInfoForDevice> dbgInfoPerDeviceMap;


	namespace dbg
	{
	

		VAL_RETURN_CODE loadDbgUtilFunctions(VkDevice device) 
		{
			dbgInfoPerDeviceMap[device].functions.setObjectName =
				(PFN_vkDebugMarkerSetObjectNameEXT)vkGetDeviceProcAddr(device, "vkDebugMarkerSetObjectNameEXT");

			dbgInfoPerDeviceMap[device].functions.setObjectTag = 
				(PFN_vkDebugMarkerSetObjectTagEXT)vkGetDeviceProcAddr(device, "vkDebugMarkerSetObjectTagEXT");

			dbgInfoPerDeviceMap[device].functions.insertMarker =
				(PFN_vkCmdDebugMarkerInsertEXT)vkGetDeviceProcAddr(device, "vkCmdDebugMarkerInsertEXT");
			
			dbgInfoPerDeviceMap[device].functions.beginMarker =
				(PFN_vkCmdDebugMarkerBeginEXT)vkGetDeviceProcAddr(device, "vkCmdDebugMarkerBeginEXT");

			dbgInfoPerDeviceMap[device].functions.endMarker =
				(PFN_vkCmdDebugMarkerEndEXT)vkGetDeviceProcAddr(device, "vkCmdDebugMarkerEndEXT");


			// check to ensure that all functions were loaded.
			for (uint32_t i = 0; i < sizeof(debugFunctions) / sizeof(PFN_vkVoidFunction*); ++i) 
			{
				const PFN_vkVoidFunction* cur_func = (PFN_vkVoidFunction*)(&dbgInfoPerDeviceMap[device].functions) + i;
				if (cur_func == VK_NULL_HANDLE)
				{
					return VAL_FAILURE;
				}
			}
			
			return VAL_SUCCESS;
		}

		VAL_RETURN_CODE initDbgUtilsForDevice(VkDevice device)
		{
			if (loadDbgUtilFunctions(device) != VAL_SUCCESS) 
			{
				dbg::printError("InitDbgUtils failed: failed to load DbgUtil functions.");
				return VAL_FAILURE;
			}
			return VAL_SUCCESS;
		}

		VAL_RETURN_CODE deinitDbgUtilsForDevice(VkDevice device)
		{
			dbgInfoPerDeviceMap.erase(device);
			return VAL_SUCCESS;
		}


		void recordVkObjectCreation(VkDevice device, void* obj)
		{
			if (obj == NULL) {
				return; // not a valid obj
			}
			auto break_it_found = objBreakOnCreateList.find(obj);
			if (break_it_found != objBreakOnCreateList.end()) {
				dbg::printNote("Breaking on object at 0x%p", obj);
				throw std::runtime_error("Breaking on object creation, see console output for details.");
			}
			dbgInfoPerDeviceMap[device].allocatedObjects.insert(obj);
		}

		void recordVkObjectDestruction(VkDevice device, void* obj)
		{
			if (obj == NULL) {
				return; // not a valid obj
			}
			auto break_it_found = objBreakOnDestroyList.find(obj);
			if (break_it_found != objBreakOnDestroyList.end()) {
				dbg::printNote("Breaking on object at void*: %p", obj);
				throw std::runtime_error("Breaking on object destruction, see console output for details.");
			}
			 
			auto allocatedObject_it = dbgInfoPerDeviceMap[device].allocatedObjects.find(obj);
			if (allocatedObject_it == dbgInfoPerDeviceMap[device].allocatedObjects.end()){
				dbg::printWarning("Attempted to record the destruction of an object whose creation has not been recorded: 0x%p.", obj);
			}
			else {
				dbgInfoPerDeviceMap[device].allocatedObjects.erase(obj);
			}
		}

		
		void setBreakOnObjectCreation(void* obj)
		{
			objBreakOnCreateList.insert(obj);
		}

		void setBreakOnObjectDestruction(void* obj)
		{
			objBreakOnDestroyList.insert(obj);
		}

		void removeBreakOnObjectCreation(void* obj)
		{
			objBreakOnCreateList.erase(obj);
		}

		void removeBreakOnObjectDestruction(void* obj)
		{
			objBreakOnDestroyList.insert(obj);
		}

		VAL_RETURN_CODE beginPassMarkerRegion(VkDevice device, VkCommandBuffer cmdbuff, std::string markerName, const float color[4])
		{
			if (!dbgInfoPerDeviceMap[device].functions.beginMarker) {
				return VAL_FAILURE;
			}
			const VkDebugMarkerMarkerInfoEXT markerInfo = {
				.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
				.pNext = NULL,
				.pMarkerName = markerName.c_str(),
				.color = *color
			};
			dbgInfoPerDeviceMap[device].functions.beginMarker(cmdbuff, &markerInfo);
			return VAL_SUCCESS;
		}

		VAL_RETURN_CODE endPassMarkerRegion(VkDevice device, VkCommandBuffer cmdbuff)
		{
			if (!dbgInfoPerDeviceMap[device].functions.endMarker) {
				return VAL_FAILURE;
			}
			dbgInfoPerDeviceMap[device].functions.endMarker(cmdbuff);
			return VAL_SUCCESS;
		}

		VAL_RETURN_CODE insertPassMarker(VkDevice device, VkCommandBuffer cmdbuff, std::string markerName, const float color[4])
		{
			if (!dbgInfoPerDeviceMap[device].functions.insertMarker) {
				return VAL_FAILURE;
			}
			const VkDebugMarkerMarkerInfoEXT markerInfo = {
				.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
				.pNext = NULL,
				.pMarkerName = markerName.c_str(),
				.color = *color
			};
			dbgInfoPerDeviceMap[device].functions.insertMarker(cmdbuff, &markerInfo);

			return VAL_SUCCESS;
		}

		/*
		VAL_RETURN_CODE nameVkObject(VkDevice device, VkCommandBuffer cmdbuff, std::string markerName, const float color[4])
		{

		}
		*/
	}
}