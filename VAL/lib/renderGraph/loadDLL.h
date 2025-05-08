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

#ifndef VAL_LOAD_DLL_H
#define VAL_LOAD_DLL_H

#ifdef __cplusplus
#define VAL_LOAD_DLL_H_BINDING extern "C"
#else
#define VAL_LOAD_DLL_H_BINDING
#endif // __cplusplus

typedef void(__cdecl* VAL_F_ADDRESS)(void);

#include <VAL/lib/VALreturnCode.h>

VAL_LOAD_DLL_H_BINDING enum VAL_RETURN_CODE VAL_initDLL_loader();

VAL_LOAD_DLL_H_BINDING enum VAL_RETURN_CODE VAL_cleanupDLL_loader();

VAL_LOAD_DLL_H_BINDING VAL_F_ADDRESS VAL_loadDLLfunction(const char* DLL_file, const char* funcName);

#endif // !VAL_LOAD_DLL_H