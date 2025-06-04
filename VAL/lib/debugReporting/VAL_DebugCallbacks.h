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

#ifndef VAL_DEBUG_CALLBACKS_H
#define VAL_DEBUG_CALLBACKS_H

#include <VAL/lib/C_compatibleBinding.h>
#include <stdbool.h>

#include <VAL/lib/debugReporting/VAL_DebugIntercept.h>

VAL_C_COMPATIBLE_BINDING void VAL_INITIALIZE_DEBUG_CALLBACK_SYSTEM();

VAL_C_COMPATIBLE_BINDING void VAL_DESTROY_DEBUG_CALLBACK_SYSTEM();

VAL_C_COMPATIBLE_BINDING void VAL_NOTE_DEBUG_ADD_INTERCEPT(VAL_DEBUG_INTERCEPT intercept);

VAL_C_COMPATIBLE_BINDING void VAL_WARN_DEBUG_ADD_INTERCEPT(VAL_DEBUG_INTERCEPT intercept);

VAL_C_COMPATIBLE_BINDING void VAL_ERROR_DEBUG_ADD_INTERCEPT(VAL_DEBUG_INTERCEPT intercept);

VAL_C_COMPATIBLE_BINDING void VAL_NOTE_DEBUG_REMOVE_INTERCEPT(VAL_DEBUG_INTERCEPT intercept);

VAL_C_COMPATIBLE_BINDING void VAL_WARN_DEBUG_REMOVE_INTERCEPT(VAL_DEBUG_INTERCEPT intercept);

VAL_C_COMPATIBLE_BINDING void VAL_ERROR_DEBUG_REMOVE_INTERCEPT(VAL_DEBUG_INTERCEPT intercept);

VAL_C_COMPATIBLE_BINDING void VAL_NOTE_DEBUG_PRINT(const char* msg);

VAL_C_COMPATIBLE_BINDING void VAL_WARN_DEBUG_PRINT(const char* msg);

VAL_C_COMPATIBLE_BINDING void VAL_ERROR_DEBUG_PRINT(const char* msg);

#endif // !VAL_DEBUG_CALLBACKS_H