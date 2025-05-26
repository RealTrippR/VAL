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

#include <VAL/lib/ext/tiny_vector.hpp>

#ifndef VAL_RENDER_GRAPH_ARG_HANDLE_LIST_HPP
#define VAL_RENDER_GRAPH_ARG_HANDLE_LIST_HPP

namespace val {

    enum class ARG_HANDLE_TYPE {
        NONE,
        READ,
        WRITE,
        READ_WRITE,
        INPUT
    };

    class ARG_HANDLE_LIST {
    public:
        tiny_vector<void*> args;

        ARG_HANDLE_TYPE type;

        template<typename... Args>
        ARG_HANDLE_LIST(Args&&... arguments) {
            type = ARG_HANDLE_TYPE::NONE;
            storeArgs(std::forward<Args>(arguments)...);
        }

    private:
        template<typename T>
        void* to_void_ptr(T&& value) {
            return static_cast<void*>(const_cast<std::remove_reference_t<T>*>(&value));
        }

        template<typename... Args>
        void storeArgs(Args&&... argsList) {
            (args.push_back(to_void_ptr(std::forward<Args>(argsList))), ...);  // fold expression
        }



    };


    class READ_ARGS : public ARG_HANDLE_LIST {
    public:
        template<typename... Args>
        READ_ARGS(Args&&... args)
            : ARG_HANDLE_LIST(std::forward<Args>(args)...) {
            type = ARG_HANDLE_TYPE::READ;
        }
    };

    class WRITE_ARGS : public ARG_HANDLE_LIST {
    public:
        template<typename... Args>
        WRITE_ARGS(Args&&... args)
            : ARG_HANDLE_LIST(std::forward<Args>(args)...) {
            type = ARG_HANDLE_TYPE::WRITE;
        }
    };

    class READ_WRITE_ARGS : public ARG_HANDLE_LIST {
    public:
        template<typename... Args>
        READ_WRITE_ARGS(Args&&... args)
            : ARG_HANDLE_LIST(std::forward<Args>(args)...) {
            type = ARG_HANDLE_TYPE::READ_WRITE;
        }
    };

    class INPUT_ARGS : public ARG_HANDLE_LIST {
    public:
        template<typename... Args>
        INPUT_ARGS(Args&&... args)
            : ARG_HANDLE_LIST(std::forward<Args>(args)...) {
            type = ARG_HANDLE_TYPE::INPUT;
        }
    };
}



#endif // !VAL_RENDER_GRAPH_ARG_HANDLE_LIST_HPP