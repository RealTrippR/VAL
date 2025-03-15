#ifndef VAL_CLEANUP_MANAGER_HPP
#define VAL_CLEANUP_MANAGER_HPP

#include <VAL/lib/system/tiedGraphicsObject.hpp>
#include <stdint.h> 
#include <stdlib.h>
#include <stdio.h>
#include <exception>
#include <stdexcept>
#include <unordered_map>
#include <queue>

namespace val {
	class cleanupManager; // forward declaration

#define CLEANUP_MANAGER_CHUNK__OBJ_COUNT 64

	class cleanupManagerChunk {
	public:
		~cleanupManagerChunk() {
			if (_objectList) {
				free(_objectList);
			}
		}
	public:
		void addObject(tiedGraphicsObject* obj, cleanupManager* parent);

	public:
		tiedGraphicsObject** _objectList = NULL;
		uint64_t _objectCount = 0;
	};

	class cleanupManager
	{
	public:
		
	public:
		void addObject(tiedGraphicsObject* obj);

		void removeObject(tiedGraphicsObject* obj);

		//void getObjects(tiedGraphicsObject** objectsListOut, uint64_t* objectCountOut);

	protected:
		friend cleanupManagerChunk;
		std::vector<cleanupManagerChunk> chunks;
		//std::vector<size_t> availableChunkIndices;
		std::queue<size_t> availableChunkIndices;
		std::unordered_map<tiedGraphicsObject*, std::pair< size_t/*CHUNK INDEX*/, uint16_t /*OBJECT INDEX*/>> objLookupMap;
	};
}
#endif // !VAL_CLEANUP_MANAGER_HPP