#include <VAL/lib/system/cleanupManager.hpp>

namespace val {
	void cleanupManagerChunk::addObject(tiedGraphicsObject* obj, cleanupManager* parent) {
#ifndef NDEBUG
		if (obj == NULL) {
			printf("VAL: Cannot add a pointer to a null object to a cleanuplist!");
			throw std::runtime_error("VAL: Cannot add a pointer to a null object to a cleanuplist!");
		}
#endif // !NDEBUG

		_objectCount++;
		if (!_objectList) {
			_objectList = (tiedGraphicsObject**)malloc(sizeof(tiedGraphicsObject*) * _objectCount);
			_objectList[0] = obj;
			goto skipMALLOCandREALLOC;
		}
		tiedGraphicsObject** tmpArr;
		if (tmpArr = (tiedGraphicsObject**)realloc(_objectList, sizeof(tiedGraphicsObject*) * _objectCount)) {
			// realloc succeeded
			_objectList = tmpArr;
			tmpArr[_objectCount - 1] = obj;
		}
		else {
			// realloc failed, use malloc instead
			tmpArr = (tiedGraphicsObject**)malloc(sizeof(tiedGraphicsObject*) * _objectCount);
			memcpy(tmpArr, _objectList, _objectCount - 1);
			tmpArr[_objectCount - 1] = obj;
			free(_objectList);
			_objectList = tmpArr;
		}

	skipMALLOCandREALLOC:
		if (_objectCount = CLEANUP_MANAGER_CHUNK__OBJ_COUNT) {
			parent->availableChunkIndices.pop();
			// if the object is full, remove from the avaible chunks
			//parent.availableChunkIndices.erase(parent.availableChunkIndices.begin() + (this - parent.chunks.data()));
		}		
	}
	//void cleanupManagerChunk::removeObject(tiedGraphicsObject* obj) {

	//}
	
	void cleanupManager::addObject(tiedGraphicsObject* obj) {
		if (availableChunkIndices.size() == 0) {
			// no chunks available, create a new chunk
			chunks.emplace_back();
			//availableChunkIndices.push_back(chunks.size()-1);
			availableChunkIndices.push(chunks.size()-1);
			chunks.back().addObject(obj, this);
		}
	}

	void cleanupManager::removeObject(tiedGraphicsObject* obj) {
	}
}