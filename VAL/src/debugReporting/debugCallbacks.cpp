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

#include <VAL/lib/debugReporting/debugCallbacks.hpp>
#include <VAL/lib/debugReporting/VAL_DebugCallbacks.h>
#include <cstdarg>

namespace val {
	namespace dbg {
		void addNoteIntercept(VAL_DEBUG_INTERCEPT intercept) {
			VAL_NOTE_DEBUG_ADD_INTERCEPT(intercept);
		}

		void addWarnIntercept(VAL_DEBUG_INTERCEPT intercept) {
			VAL_WARN_DEBUG_ADD_INTERCEPT(intercept);
		}

		void addErrorIntercept(VAL_DEBUG_INTERCEPT intercept) {
			VAL_ERROR_DEBUG_ADD_INTERCEPT(intercept);
		}

		void removeNoteIntercept(VAL_DEBUG_INTERCEPT intercept) {
			VAL_NOTE_DEBUG_REMOVE_INTERCEPT(intercept);
		}

		void removeWarnIntercept(VAL_DEBUG_INTERCEPT intercept) {
			VAL_WARN_DEBUG_REMOVE_INTERCEPT(intercept);
		}

		void removeErrorIntercept(VAL_DEBUG_INTERCEPT intercept) {
			VAL_ERROR_DEBUG_REMOVE_INTERCEPT(intercept);
		}

		void printNote(const char* format_msg, ...) {
			char msg[512];
			va_list args;
			va_start(args, format_msg);
			vsnprintf(msg, sizeof(msg) , format_msg, args);
			va_end(args);
			VAL_NOTE_DEBUG_PRINT(msg);
		}

		void printWarning(const char* format_msg, ...) {
			char msg[512];
			va_list args;
			va_start(args, format_msg);
			vsnprintf(msg, sizeof(msg), format_msg, args);
			va_end(args);
			VAL_WARN_DEBUG_PRINT(msg);
		}

		void printError(const char* format_msg, ...) {
			char msg[512];
			va_list args;
			va_start(args, format_msg);
			vsnprintf(msg, sizeof(msg), format_msg, args);
			va_end(args);
			VAL_ERROR_DEBUG_PRINT(msg);
		}
	}
}