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

#ifndef TINY_VECTOR_HPP
#define TINY_VECTOR_HPP

#include <stdint.h>
#include <stdexcept>
#include <new>
#include <stdlib.h>
#include <initializer_list>
#include <memory>

// @brief A small vector class optimized for vectors where the size does not change frequently.
// 
// This is a minimalistic implementation of a vector. It does not have any value for capacity,
// therefore the size must always match the amount of allocated data. This makes it rather inefficient
// for lists that are frequently resized.
// 
// @tparam t The type of the elements stored in the vector.
// @tparam size_type The type used for size; default is uint32_t.
template <typename t, typename size_type = uint32_t>
class tiny_vector {
public:
    tiny_vector() = default;

    tiny_vector(size_type size) {
        resize(size);
    }


    tiny_vector(size_type size, std::initializer_list<t> list) {
        resize(size);
        size_type i = 0;
        for (const t& value : list) {
            _data[i++] = value;
        }
    }

    tiny_vector(std::initializer_list<t> list) {
        resize(list.size());
        size_type i = 0;
        for (const t& value : list) {
            _data[i++] = value;
        }
    }

    tiny_vector(tiny_vector<t>&& other) {
        _size = other._size;
        _data = other._data;
        other._data = NULL;
        other._size = 0u;
    }

    ~tiny_vector() {
        clear();
    }

    // assignment
    t& operator[](size_t index) {
#ifndef NDEBUG
        if (_size == 0u || index > _size - 1) {
            throw std::out_of_range("BAD ACCESS: index exceeds vector size");
        }
#endif // !NDEBUG

        return _data[index];
    }

    // getter
    const t& operator[](size_t index) const {
        return get(index);
    }

    tiny_vector<t>& operator=(std::initializer_list<t> list) {
        resize(list.size());
        std::copy(list.begin(), list.end(), _data);
        return *this;
    }

    tiny_vector<t>& operator=(const tiny_vector<t>& other) {
        if (this != &other) {  // Prevent self-assignment
            resize(other._size);
            for (size_type i = 0; i < other._size; ++i) {
                _data[i] = other._data[i];
            }
        }
        return *this;
    }

    void push_back(const t& val) {
        resize(_size + 1);
        back() = val;
    }

    /**********************************************/
    /* iterators */
public:
    class iterator {
    public:
        using value_type = t;
        using pointer = t*;
        using reference = t&;
        using difference_type = ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;

        iterator(t* ptr) : _ptr(ptr) {}

        // Dereference operators
        reference operator*() const { return *_ptr; }
        pointer operator->() const { return _ptr; }

        // Increment/Decrement
        iterator& operator++() { ++_ptr; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++_ptr; return tmp; }
        iterator& operator--() { --_ptr; return *this; }
        iterator operator--(int) { iterator tmp = *this; --_ptr; return tmp; }


        // Comparison operators
        bool operator==(const iterator& other) const { return _ptr == other._ptr; }
        bool operator!=(const iterator& other) const { return _ptr != other._ptr; }
        bool operator<(const iterator& other) const { return _ptr < other._ptr; }
        bool operator>(const iterator& other) const { return _ptr > other._ptr; }
        bool operator<=(const iterator& other) const { return _ptr <= other._ptr; }
        bool operator>=(const iterator& other) const { return _ptr >= other._ptr; }

        // Assignment (for erasing values)
        void operator=(const t& val) const { *_ptr = val; }

        // Random access
        iterator& operator+=(difference_type n) { _ptr += n; return *this; }
        iterator& operator-=(difference_type n) { _ptr -= n; return *this; }
        iterator operator+(difference_type n) const { return iterator(_ptr + n); }
        iterator operator-(difference_type n) const { return iterator(_ptr - n); }
        difference_type operator-(const iterator& other) const { return _ptr - other._ptr; }

        reference operator[](difference_type n) { return *(_ptr + n); }

    private:
        t* _ptr;
    };



public:
    // Begin function returning a non-const iterator to the start
    iterator begin() {
        return iterator(_data);
    }

    // End function returning a non-const iterator to the end - note that according to the C++ standard, the end is just beyond the last valid element
    iterator end() {
        return iterator(_data + _size);
    }


public:
    inline void clear() {
        if (!_data) { return; }
        for (size_t i = 0; i < _size; ++i) {
            (_data + i)->~t(); // destruct
        }
        free(_data);
        _data = NULL;
        _size = 0u;
    }

    // setters
    inline void resize(const size_type newSize) {
        if (newSize == _size) return;

        // the vec has not yet been allocated
        if (!_data) {
            _data = (t*)malloc(newSize * sizeof(t));
            if (!_data) throw std::bad_alloc();

            for (size_type i = 0; i < newSize; ++i) {
                new (_data + i) t(); // call constructors
            }
        }
        else
        {
            // use realloc
            if (newSize > 0) {
                t* tmp = (t*)realloc(_data, sizeof(t) * newSize);
                if (!tmp) {
                    // realloc failed
                    tmp = _data; // we'll free this when we're done
                    _data = (t*)malloc(newSize * sizeof(t));
                    if (!_data) throw std::bad_alloc();
                    // allocate and copy the old memory
                    memcpy(_data, tmp, _size * sizeof(t));

                    free(tmp);
                }
                else {
                    // realloc succeeded, assign _data to tmp I know that it seems redundant, but if
                    // realloc can't find space it may destroy the old block and then
                    // make a call to malloc (depending on it's implementation)
                    _data = tmp;
                }
                // alloc succeeded, call constructors
                for (size_type i = _size; i < newSize; ++i) {
                    new (_data + i) t();
                }
            }
        }
        // call destructors if needed
        if (_size > newSize) {
            for (size_type i = 0; i < _size - newSize; ++i) {
                (_data + newSize + i - 1)->~t(); // destruct
            }
        }
        // update size
        _size = newSize;
    }


    void erase(size_type index) {
        if (index >= _size) {
            throw std::out_of_range("BAD ACCESS: index exceeds vector size");
        }

        // Shift all elements after the erased element to the left
        for (size_type i = index; i < _size - 1; ++i) {
            _data[i] = std::move(_data[i + 1]);
        }

        resize(_size - 1);
    }

    void erase(iterator pos) {
        if (pos >= end() || pos < begin()) {
            throw std::out_of_range("BAD ACCESS: iterator out of range");
        }
        // Shift elements left
        for (iterator it = pos; it + 1 != end(); ++it) {
            *it = *(it + 1);
        }

        resize(_size - 1);
        return pos;
    }


    iterator erase(iterator first, iterator last) {
        if (first > last || first < begin() || last > end()) {
            throw std::out_of_range("erase: invalid range");
        }

        // Calculate the range size
        size_type range_size = last - first;

        iterator it = first;
        while (last != end()) {
            // Directly move elements without calling any constructor/destructor
            *it = std::move(*last);
            ++it;
            ++last;
        }

        resize(_size - range_size);
        return first;
    }

public:

    // getters
    t* data() {
        return (t*)_data;
    }

    bool empty() const {
        return _size == 0;
    }

    size_type size() const {
        return _size;
    }

    t& front() const {
        return *(_data);
    }

    t& back() const {
        return *(_data + (_size - 1));
    }
private:
    const t& get(const size_type idx) const {
#ifndef NDEBUG
        if (_size == 0u || idx > _size - 1) {
            throw std::out_of_range("BAD ACCESS: index exceeds vector size");
        }
#endif // !NDEBUG
        return _data[idx];
    }

    t* _data = NULL;
    size_type _size = 0u;
};


#endif // !TINY_VECTOR_HPP
