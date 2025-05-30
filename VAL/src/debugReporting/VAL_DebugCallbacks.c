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

#include <VAL/lib/debugReporting/VAL_DebugCallbacks.h>
#include <VAL/lib/ext/hashmap.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <io.h>

struct hashmap* noteInterceptMap = NULL;
struct hashmap* warnInterceptMap = NULL;
struct hashmap* errInterceptMap = NULL;

int VAL_intercept_compare(const void* a, const void* b, void* udata)
{
	VAL_DEBUG_INTERCEPT fa = *(VAL_DEBUG_INTERCEPT*)a;
	VAL_DEBUG_INTERCEPT fb = *(VAL_DEBUG_INTERCEPT*)b;

	return (fa > fb) - (fa < fb);
}

uint64_t VAL_intercept_hash(const void* item, uint64_t seed0, uint64_t seed1)
{
	return hashmap_xxhash3(item, sizeof(VAL_DEBUG_INTERCEPT), seed0, seed1);
}

void VAL_INITIALIZE_DEBUG_CALLBACK_SYSTEM() 
{		

	if (!noteInterceptMap) {
		noteInterceptMap = (struct hashmap*)hashmap_new(sizeof(VAL_DEBUG_INTERCEPT), 0, 0, 0,
			VAL_intercept_hash, VAL_intercept_compare, NULL, NULL);
	}
	if (!warnInterceptMap) {
		warnInterceptMap = hashmap_new(sizeof(VAL_DEBUG_INTERCEPT), 0, 0, 0,
			VAL_intercept_hash, VAL_intercept_compare, NULL, NULL);
	}
	if (!errInterceptMap) {
		errInterceptMap = hashmap_new(sizeof(VAL_DEBUG_INTERCEPT), 0, 0, 0,
			VAL_intercept_hash, VAL_intercept_compare, NULL, NULL);
	}
}

void VAL_DESTROY_DEBUG_CALLBACK_SYSTEM() 
{
	if (noteInterceptMap) {
		hashmap_free(noteInterceptMap);
	}
	if (warnInterceptMap) {
		hashmap_free(warnInterceptMap);
	}
	if (errInterceptMap) {
		hashmap_free(errInterceptMap);
	}
}

void VAL_NOTE_DEBUG_ADD_INTERCEPT(VAL_DEBUG_INTERCEPT intercept) 
{
	VAL_INITIALIZE_DEBUG_CALLBACK_SYSTEM();
	hashmap_set(noteInterceptMap, &intercept);
}

void VAL_WARN_DEBUG_ADD_INTERCEPT(VAL_DEBUG_INTERCEPT intercept) 
{
	VAL_INITIALIZE_DEBUG_CALLBACK_SYSTEM();
	hashmap_set(warnInterceptMap, &intercept);
}

void VAL_ERROR_DEBUG_ADD_INTERCEPT(VAL_DEBUG_INTERCEPT intercept) 
{
	VAL_INITIALIZE_DEBUG_CALLBACK_SYSTEM();
	hashmap_set(errInterceptMap, &intercept);
}

void VAL_NOTE_DEBUG_REMOVE_INTERCEPT(VAL_DEBUG_INTERCEPT intercept) 
{
	if (!noteInterceptMap) {
		printf("VAL: Failed to remove note intercept, no note intercepts have been added!\n");
		return;
	}
	hashmap_delete(noteInterceptMap, &intercept);
}

void VAL_WARN_DEBUG_REMOVE_INTERCEPT(VAL_DEBUG_INTERCEPT intercept) 
{
	if (!warnInterceptMap) {
		printf("VAL: Failed to remove warn intercept, no warn intercepts have been added!\n");
		return;
	}
	hashmap_delete(warnInterceptMap, &intercept);
}

void VAL_ERROR_DEBUG_REMOVE_INTERCEPT(VAL_DEBUG_INTERCEPT intercept) 
{
	if (!errInterceptMap) {
		printf("VAL: Failed to remove error intercept, no error intercepts have been added!\n");
		return;
	}
	hashmap_delete(errInterceptMap, &intercept);
}

void VAL_NOTE_DEBUG_PRINT(const char* msg)
{
	bool blockPrint = false;
	size_t iter = 0;
	VAL_DEBUG_INTERCEPT* intercept;
	if (noteInterceptMap) {
		while (hashmap_iter(noteInterceptMap, &iter, &intercept)) {
			if (!(*intercept)) {
				printf("VAL: Debug note intercept #%d is null\n", iter);
			}
			else {
				bool bpCpy = false;
				(*intercept)(msg, &bpCpy);
				if (bpCpy) {
					blockPrint = true;
				}
			}
		}
	}

	if (blockPrint == false) {
		fputs("VAL - NOTE: ", stderr);
		fputs(msg, stderr);
		fputs("\n", stderr);
	}
}

void VAL_WARN_DEBUG_PRINT(const char* msg)
{
	bool blockPrint = false;
	size_t iter = 0;
	VAL_DEBUG_INTERCEPT* intercept;
	if (warnInterceptMap) {
		while (hashmap_iter(warnInterceptMap, &iter, &intercept)) {
			if (!intercept) {
				printf("VAL: Debug warn intercept #%d is null\n", iter);
			}
			else {
				bool bpCpy = false;
				(*intercept)(msg, &bpCpy);
				if (bpCpy) {
					blockPrint = true;
				}
			}
		}
	}

	if (blockPrint == false) {
		fputs("VAL - WARNING: ", stderr);
		fputs(msg, stderr);
		fputs("\n", stderr);
	}
}

void VAL_ERROR_DEBUG_PRINT(const char* msg)
{
	bool blockPrint = false;
	size_t iter = 0;
	VAL_DEBUG_INTERCEPT* intercept;
	if (errInterceptMap) {
		while (hashmap_iter(errInterceptMap, &iter, &intercept)) {
			if (!intercept) {
				printf("VAL: Debug error intercept #%d is null\n", iter);
			}
			else {
				bool bpCpy = false;
				(*intercept)(msg, &bpCpy);
				if (bpCpy) {
					blockPrint = true;
				}
			}
		}
	}

	if (blockPrint == false) {
		fputs("VAL - ERROR: ", stderr);
		fputs(msg, stderr);
		fputs("\n", stderr);
	}
}