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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <VAL/lib/ext/hashmap.h>
#include <VAL/lib/renderGraph/loadDLL.h>

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <dlfcn.h> 
#else
#endif

typedef void* DLL_PTR;

struct DLL_HANDLE {
    DLL_PTR* DLL;
    char* DLLfilepath; // the dll handle owns this and it must be freed upon destruction
};

// this hashmap contains pointers to allocated DLL_Handles
struct hashmap* loadedDLLmap;

wchar_t* ascii_to_unicode(const char* ascii_str) {
    if (!ascii_str) return NULL;

    // Determine size (including null terminator)
    // https://en.cppreference.com/w/c/string/multibyte/mbsrtowcs
    size_t len = 0;
    mbstowcs_s(&len, NULL, 0, ascii_str, 0);

    if (len == 0) {
#ifndef NDEBUG
        printf("VAL: ERROR: Failed to convert ascii to unicode");
#endif // !NDEBUG
        return NULL;
    }

    // malloc wstr
    const size_t buffSize = (len + 1) * sizeof(wchar_t);
    wchar_t* unicode_str = (wchar_t*)malloc(buffSize);
    unicode_str[buffSize - 1] = '\0'; // null terminate as a backup
    if (!unicode_str) {
        return NULL;
    }

    // convert to wstr
    mbstowcs_s(NULL, unicode_str, len, ascii_str, len - 1);

    return unicode_str;
}

int DLL_HANDLE_compare(const struct DLL_HANDLE* a, const struct DLL_HANDLE* b, void* udata) {
    const struct DLL_HANDLE** da = a;
    const struct DLL_HANDLE** db = b;
    return strcmp((*da)->DLLfilepath, (*db)->DLLfilepath);
}

uint64_t DLL_HANDLE_hash(const void** item, uint64_t seed0, uint64_t seed1) {
    const struct DLL_HANDLE** hdl = item;
    char* DLLfilepath = NULL;
    uint16_t DLLpathLen = 0u;
    if (*hdl) {
        DLLfilepath = (*hdl)->DLLfilepath;
        DLLpathLen = strlen((*hdl)->DLLfilepath);
    }
    return hashmap_sip(DLLfilepath, DLLpathLen, seed0, seed1);
}



bool VAL_isDLL_loaderInitialized() {
    return loadedDLLmap;
}

enum VAL_RETURN_CODE VAL_initDLL_loader() {
    if (loadedDLLmap == NULL) {
        loadedDLLmap = hashmap_new(sizeof(struct DLL_HANDLE*), 0, 0, 0,
            DLL_HANDLE_hash, DLL_HANDLE_compare, NULL, NULL);
        if (loadedDLLmap == NULL) {
            return VAL_FAILURE;
        }
        else {
            return VAL_SUCCESS;
        }
    }
    return VAL_SUCCESS;
}

enum VAL_RETURN_CODE VAL_cleanupDLL_loader() {
    enum VAL_RETURN_CODE retcode = VAL_SUCCESS;
    size_t iter = 0;
    void* item;
    if (loadedDLLmap) {
        while (hashmap_iter(loadedDLLmap, &iter, &item)) {
            const struct DLL_HANDLE** hdl = item;
            if ((*hdl)->DLLfilepath != NULL) {
                free((*hdl)->DLLfilepath);
            }
            if ((*hdl)->DLL != NULL) {
#ifdef _WIN32
                int fFreeResult = FreeLibrary((*hdl)->DLL);
                if (fFreeResult == 0) { // https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-freelibrary
                    fprintf(stderr, "VAL: ERROR: Could not free library, error id: %d\n", fFreeResult);
                    retcode = VAL_FAILURE;
                }
#elif __linux__
                int fFreeResult = dlclose((*hdl)->DLL);
                if (dlclose(handle) != 0) {
                    fprintf(stderr, "VAL: ERROR: Could not free library: %s\n", dlerror());
                    retcode = VAL_FAILURE;
                }
#endif // _WIN32

            }
            free(*hdl);
        }
    }

    // free items inside hashmap the and the dll.DLLfilepath string
    hashmap_free(loadedDLLmap);

    return retcode;
}

// if the dll handle is cached, it will not be loaded and instead the handle of that DLL will be returned
struct DLL_HANDLE* loadDLLtoCache(const char* DLL_filepath) {
    struct DLL_HANDLE* dll = NULL;

    struct DLL_HANDLE** tmp_dll_hdl = (struct DLL_HANDLE**)hashmap_get(loadedDLLmap, &(struct DLL_HANDLE){.DLLfilepath = DLL_filepath});
    if (tmp_dll_hdl) { // the dll handle has already been cached
        dll = *tmp_dll_hdl;
        return dll;
    }
    else {
        dll = malloc(sizeof(struct DLL_HANDLE));
#ifdef _WIN32
        const wchar_t* dirFilepathAsWSTR = ascii_to_unicode(DLL_filepath);
        dll->DLL = (DLL_PTR)LoadLibrary(dirFilepathAsWSTR);
        if (!dirFilepathAsWSTR) {
            return NULL;
        }
        else {
            free(dirFilepathAsWSTR);
        }
#endif // _WIN32
#ifdef __linux__
        dll->DLL = (DLL_PTR)dlopen(DLL_filepath, RTLD_NOW);
#endif // __linux__


        if (!dll->DLL) { // failed to load lib
            dll->DLLfilepath = NULL;
            free(dll);
            return NULL;
        }

        const uint32_t nameLen = strlen(DLL_filepath);
        dll->DLLfilepath = malloc(nameLen + 1);
        strcpy_s(dll->DLLfilepath, nameLen + 1, DLL_filepath);

        hashmap_set(loadedDLLmap, &dll);
    }

    return dll;
}

// returns null if it fails
// if hinstLib is passed as NULL it will be ignored, otherwise it is used to pass a handle to an already loaded library
VAL_F_ADDRESS VAL_loadDLLfunction(const char* DLL_file, const char* funcName) {
    VAL_F_ADDRESS funcAdd = NULL;


    // Get a handle to the DLL module.
    struct DLL_HANDLE* dllHdl = loadDLLtoCache(DLL_file);
    //hinstLib = LoadLibrary(DLL_file);

    // If the handle is valid, try to get the function address.
    if (dllHdl != NULL)
    {
#ifdef _WIN32
        funcAdd = (VAL_F_ADDRESS)GetProcAddress((HINSTANCE)dllHdl->DLL, funcName);
#elif __linux__
        funcAdd = (VAL_F_ADDRESS)dlsym(dllHdl->DLL, funcName);
#endif // 
        return funcAdd;
    }
    else {
        return NULL;
    }
}



/************************************************************************/

// a sentinel to determine when to create or destroy the DLL loader
uint16_t VAL_dllLoaderRefCount = 0u;

void VAL_incDLLloaderRefCount() {
    if (VAL_dllLoaderRefCount == 0) {
        VAL_initDLL_loader();
    }
    VAL_dllLoaderRefCount++;
}
void VAL_decDLLloaderRefCount() {
    // prevent integer underflow and deallocate resources
    if (VAL_dllLoaderRefCount > 0) {
        VAL_dllLoaderRefCount--;
        VAL_cleanupDLL_loader();
    }
}