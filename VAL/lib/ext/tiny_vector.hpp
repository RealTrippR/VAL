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
#include <limits> // Required for std::numeric_limits

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

    static_assert(!std::is_signed<size_type>::value, "tiny_vector: size_type must be unsigned");

    tiny_vector() 
    {
        _size = 0u;
        _data = NULL;
    }

    tiny_vector(size_type size) {
        resize(size);
    }

    template <typename U = t, typename = std::enable_if_t<std::is_trivially_copyable<U>::value>>
    tiny_vector(const char* cstr) {
        const uint64_t len = std::strlen(cstr);
        if ((len + 1) > static_cast<size_t>(-1)) {
            return; // Prevent overflow
        }
        resize(static_cast<size_type>(len + 1));
        std::memcpy(_data, cstr, len + 1); // Safe because t is trivially copyable
    }

    tiny_vector(size_type size, const t& value) {
        resize(size);
        for (size_type i = 0; i < _size; ++i) {
            _data[i] = value;
        }
    }

    tiny_vector(size_type size, std::initializer_list<t> list) {
        resize(size);
        size_type i = 0;
        for (const t& value : list) {
            _data[i++] = value;
        }
    }

    tiny_vector(const tiny_vector<t>& other ) {
        resize(other.size());
        for (size_t i = 0; i < other.size(); ++i) {
            _data[i] = other._data[i];
        }
    }

    tiny_vector(std::initializer_list<t> list) {
#ifndef NDEBUG
        if (list.size() > max_size())
        {
            throw std::out_of_range("tiny_vector: The size of std::initializer_list<t> list exceeds the maximum size of this tiny_vector.");
        }
#endif // !NDEBUG

        resize((size_type)list.size());
        size_type i = 0;
        for (const t& value : list) {
            _data[i++] = value;
        }
    }

    tiny_vector(tiny_vector<t>&& other) noexcept {
        _size = other._size;
        _data = other._data;
        other._data = NULL;
        other._size = 0u;
    }

    ~tiny_vector() {
        clear();
    }

    // assignment
    t& operator[](size_type index) {
#ifndef NDEBUG
        if (_size == 0u || index > _size - 1) {
            throw std::out_of_range("BAD ACCESS: index exceeds vector size");
        }
#endif // !NDEBUG

        return _data[index];
    }

    // assignment to cstr
    template <typename U = t>
    std::enable_if_t<std::is_trivially_copyable<U>::value, tiny_vector&>
        operator=(const char* cstr) {
        const size_t len = std::strlen(cstr);
        if ((len + 1) > static_cast<size_t>(-1)) return *this;
        resize(static_cast<size_type>(len + 1));
        std::memcpy(_data, cstr, len + 1); // Assumes U is 1-byte
        return *this;
    }

    // getter
    const t& operator[](size_type index) const {
        return get(index);
    }

    tiny_vector<t>& operator=(std::initializer_list<t> list) {
        resize(size_type(list.size()));
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

    void pop_back() {
        resize(_size - 1);
    }

    template<typename... Args>
    t& emplace_back(Args&&... args) {
        growNoConstructor();
        new(&_data[_size-1]) t(std::forward<Args>(args)...);
        return _data[_size - 1];
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
        iterator& operator++() { ++_ptr; return *this; } //++itr
        iterator operator++(int) { iterator tmp = *this; ++_ptr; return tmp; } //itr++
        iterator& operator--() { --_ptr; return *this; } //--itr
        iterator operator--(int) { iterator tmp = *this; --_ptr; return tmp; } //itr--


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

    class const_iterator {
    public:
        using value_type = t;
        using pointer = t*;
        using reference = t&;
        using difference_type = ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;

        const_iterator(t* ptr) : _ptr(ptr) {}

        // Dereference operators
        reference operator*() const { return *_ptr; }
        pointer operator->() const { return _ptr; }

        // Comparison operators
        bool operator==(const const_iterator& other) const { return _ptr == other._ptr; }
        bool operator!=(const const_iterator& other) const { return _ptr != other._ptr; }
        bool operator<(const const_iterator& other) const { return _ptr < other._ptr; }
        bool operator>(const const_iterator& other) const { return _ptr > other._ptr; }
        bool operator<=(const const_iterator& other) const { return _ptr <= other._ptr; }
        bool operator>=(const const_iterator& other) const { return _ptr >= other._ptr; }

        // Increment/Decrement
        const_iterator& operator++() { ++_ptr; return *this; } // ++itr
        const_iterator operator++(int) { const_iterator tmp = *this; ++_ptr; return tmp; } //itr++
        const_iterator& operator--() { --_ptr; return *this; } //--itr
        const_iterator operator--(int) { const_iterator tmp = *this; --_ptr; return tmp; } //itr--

        // Random access
        const_iterator& operator+=(difference_type n) { _ptr += n; return *this; }
        const_iterator& operator-=(difference_type n) { _ptr -= n; return *this; }
        const_iterator operator+(difference_type n) const { return const_iterator(_ptr + n); }
        const_iterator operator-(difference_type n) const { return const_iterator(_ptr - n); }
        difference_type operator-(const const_iterator& other) const { return _ptr - other._ptr; }

        reference operator[](difference_type n) { return *(_ptr + n); }
    private:
        t* _ptr;
    };


public:
    // iterator constructors
    tiny_vector(iterator begin, iterator end)
    {
        const size_t count = static_cast<size_t>(end - begin);
        resize(count);
        std::copy(begin, end, _data);
    }


public:
    // Begin function returning a non-const iterator to the start
    iterator begin() noexcept {
        return iterator(_data);
    }

    // End function returning a non-const iterator to the end - note that according to the C++ standard, the end is just beyond the last valid element
    iterator end() noexcept {
        return iterator(_data + _size);
    }

    // Begin function returning a const iterator to the start
    const_iterator begin() const noexcept {
        return const_iterator(_data);
    }
    
    // End function returning a const iterator to the end - note that according to the C++ standard, the end is just beyond the last valid element
    const_iterator end() const noexcept {
        return const_iterator(_data + _size);
    }

    // Begin function returning a const iterator to the start
    const_iterator cbegin() const {
        return const_iterator(_data);
    }

    // End function returning a const iterator to the end - note that according to the C++ standard, the end is just beyond the last valid element
    const_iterator cend() const {
        return const_iterator(_data + _size);
    }

public:
    inline void insert(iterator it, const t& val) {
        resize(size() + 1);
        *it = val;
    }

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

    iterator erase(iterator pos) {
        if (pos >= end() || pos < begin()) {
            throw std::out_of_range("BAD ACCESS: iterator out of range");
        }
        // Shift elements left
        for (iterator it = pos; it + 1 != end(); ++it) {
            *it = *(it + 1);
        }

        resize(_size - 1);
        return begin() + _size;
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
    t* data() noexcept {
        return (t*)_data;
    }

    // getters
    const t* data() const noexcept {
        return (t*)_data;
    }

    bool empty() const noexcept {
        return _size == 0;
    }

    size_type size() const noexcept {
        return _size;
    }

    constexpr size_type max_size() const noexcept {
        return std::numeric_limits<size_type>::max();
    }

    t& front() const {
        return *(_data);
    }

    t& back() const {
        return *(_data + (_size - 1));
    }

    inline t& emplaceBack() {
        resize(_size + 1);
        return back();
    }

private:

    void growNoConstructor() 
    {
        const size_type newSize = _size + 1;

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
            _data = tmp;
        }
        _size = newSize;
    }


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