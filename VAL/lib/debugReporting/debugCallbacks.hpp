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

#ifndef VAL_DEBUG_CALLBACKS_HPP
#define VAL_DEBUG_CALLBACKS_HPP

#include <iostream>
#include <VAL/lib/debugReporting/VAL_DebugIntercept.h>

namespace val 
{
	namespace dbg 
	{

		void addNoteIntercept(VAL_DEBUG_INTERCEPT intercept);

		void addWarnIntercept(VAL_DEBUG_INTERCEPT intercept);

		void addErrorIntercept(VAL_DEBUG_INTERCEPT intercept);

		void removeNoteIntercept(VAL_DEBUG_INTERCEPT intercept);

		void removeWarnIntercept(VAL_DEBUG_INTERCEPT intercept);

		void removeErrorIntercept(VAL_DEBUG_INTERCEPT intercept);

		void printNote(const char* format_msg, ...);

		void printWarning(const char* format_msg, ...);

		void printError(const char* format_msg, ...);
	}
}

#endif // !VAL_DEBUG_CALLBACKS_HPP