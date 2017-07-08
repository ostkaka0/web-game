// Copyrighted (c) John Emanuelsson 2016 - All rights reserved
// Created November the 17th, 2016

#pragma once

#include <string.h>
#include <stdlib.h>
#include <assert.h>

//namespace game {

template<typename T>
int array_less_function(T a, T b) {
    return (int)(a - b);
}

template<typename T>
struct Array {
    T* data;
    size_t byte_capacity;
    size_t byte_length;

    inline void init();
    inline void destroy();
    inline bool push(const T value);
    inline T pop();
    inline bool insert(size_t index, T val);
    inline void swap(size_t index_a, size_t index_b);
    inline bool resize(size_t length, size_t value = 0);
    inline void splice(size_t start, size_t num);
    inline void shrink(size_t length);
    inline bool grow(size_t num, size_t value = 0);
    inline void clear();
    inline bool reserve(size_t new_capacity);
    inline bool compact();
    inline bool push_array(T* array, size_t num);
    inline bool extend(Array<T> other);
    template<int(*compare)(T, T) = array_less_function<T>>
    size_t binary_search(T val, int begin = 0, int  end = -1);
    template<int(*compare)(T, T) = array_less_function<T>>
    size_t binary_insert(T val);
    template<int(*compare)(T, T) = array_less_function<T>>
    size_t binary_erase(T val);
    template<int(*compare)(T, T) = array_less_function<T>>
    size_t heap_insert(T val);
    template<int(*compare)(T, T) = array_less_function<T>>
    T heap_erase(size_t index = 0/*, Compare_Function compare_function = [](T a, T b) { return a - b; }*/);

    inline T& operator[](size_t index);
    inline T first() const;
    inline T last() const;
    inline size_t length() const;
};

template<typename T>
static inline bool array_expand(Array<T>* array, size_t size = 1);

template<typename T>
void Array<T>::init() {
    memset(this, 0, sizeof(*this));
}

template<typename T>
void Array<T>::destroy() {
    free(this->data);
    memset(this, 0, sizeof(*this));
}

template<typename T>
bool Array<T>::push(const T value) {
    if (!array_expand(this)) return false;
    this->data[byte_length / sizeof(T)] = value; //*(T*)(((char*)this->data) + byte_length) = value;
    this->byte_length += sizeof(T);
    return true;
}

template<typename T>
T Array<T>::pop() {
    assert(this->byte_length <= this->byte_capacity);
    this->byte_length -= sizeof(T);
    return this->data[this->length()];
}

template<typename T>
bool Array<T>::insert(size_t index, T value) {
    assert(index <= this->length() && index >= 0);
    if (!array_expand(this)) return false;
    memmove(&this->data[index+1], &this->data[index], this->byte_length - index * sizeof(T));
    this->data[index] = value;
    this->byte_length += sizeof(T);
    return true;
}

template<typename T>
void Array<T>::swap(size_t index_a, size_t index_b) {
    assert(index_a < this->length() && index_a >= 0);
    assert(index_b < this->length() && index_b >= 0);
    T temp = this->data[index_a];
    this->data[index_a] = this->data[index_b];
    this->data[index_b] = temp;
}

template<typename T>
bool Array<T>::resize(size_t length, size_t value) {
    assert(length != -1);
    if (length && !this->reserve(length)) return false;
    if (length > this->length()) memset((u8*)this->data + this->byte_length, value, length * sizeof(T) - this->byte_length);
    this->byte_length = length * sizeof(T);
    return true;
}


template<typename T>
void Array<T>::splice(size_t start, size_t num) {
    assert(this->byte_length <= this->byte_capacity);
    memmove(&this->data[start], &this->data[start + num], (this->length() - start - num) * sizeof(T));
    this->byte_length -= num * sizeof(T);
}

template<typename T>
void Array<T>::shrink(size_t length) {
    assert(length >= 0);
    this->byte_length = length < this->length() ? length * sizeof(T) : this->byte_length;
}

template<typename T>
bool Array<T>::grow(size_t num, size_t value) {
    assert(num > 0);
    return this->resize(this->length() + num, value);
}

template<typename T>
void Array<T>::clear() {
    this->byte_length = 0;
}

template<typename T>
bool Array<T>::reserve(size_t new_capacity) {
    assert(new_capacity > 0);
    if (new_capacity * sizeof(T) > this->byte_capacity) {
        T* ptr = (T*)(realloc(this->data, new_capacity * sizeof(T)));
        if (ptr == NULL) return false;
        this->data = ptr;
        this->byte_capacity = new_capacity * sizeof(T);
    }
    return true;
}

template<typename T>
bool Array<T>::compact() {
    if (this->byte_length == 0) {
        free(this->data);
        this->data = NULL;
        this->byte_capacity = 0;
    } else {
        T* ptr = (T*)realloc(this->data, this->byte_length);
        if (ptr == NULL) return false;
        this->data = ptr;
        this->byte_capacity = this->byte_length;
    }
    return true;
}

template<typename T>
bool Array<T>::push_array(T* array, size_t num) {
    assert(array != NULL && num >= 0);
    if (!this->reserve(this->length() + num))
        return false;
    memcpy((char*)this->data + this->byte_length, array, num * sizeof(T));
    this->byte_length += num * sizeof(T);
    return true;
}

template<typename T>
bool Array<T>::extend(Array<T> other) {
    return this->push_array(other.data, other.length());
}


template<typename T>
inline T& Array<T>::operator[](size_t index) {
    assert(index < this->length() && index >= 0);
    return this->data[index];
}

template<typename T>
inline T Array<T>::first() const {
    assert(this->byte_length > 0);
    return this->data[0];
}

template<typename T>
inline T Array<T>::last() const {
    assert(this->byte_length > 0);
    return this->data[this->length()-1];
}

template<typename T>
inline size_t Array<T>::length() const {
    return this->byte_length / sizeof(T);
}

template<typename T>
template<int(*compare)(T, T)>
size_t Array<T>::binary_search(T val, int begin, int end) {
    if (end == -1) end = this->length() - 1;
    while (begin <= end) {
        int middle = begin + (end - begin >> 1);
        int compare_value = compare(val, this->data[middle]);
        if (compare_value > 0) begin = middle + 1;
        else if (compare_value < 0) end = middle - 1;
        else return middle;
    }
    return begin;
    /*if (begin == end) {
        if (begin != -1) return begin;
        if (this->length() == 0) return 0;
        begin = 0;
        end = this->length();
    }
    size_t middle = begin + end - 1 >> 1;
    size_t compare_value = compare(this->data[middle]);
    if (compare_value < 0) return this->binary_search<compare>(val, middle+1, end);
    if (compare_value > 0) return this->binary_search<compare>(val, begin, middle);
    return middle;*/
}

template<typename T>
template<int(*compare)(T, T)>
size_t Array<T>::binary_insert(T val) {
    if (this->length() == 0) this->push(val);
    size_t index = this->binary_search<compare>(val);
    this->insert(index, val);
    return index;
}

template<typename T>
template<int(*compare)(T, T)>
size_t Array<T>::binary_erase(T val) {
    assert(this->length());
    size_t index = this->binary_search<compare>(val);
    assert(this->data[index] == val);
    this->splice(index, 1);
    return index;
}

template<typename T>
template<int(*compare)(T, T)>
size_t Array<T>::heap_insert(T val) {
    size_t index = this->length();
    this->push(val);
    while (index) {
        size_t parent_index = index >> 1;
        if (compare(this->data[index], this->data[parent_index]) >= 0) break;
        T temp = this->data[index];
        this->data[index] = this->data[parent_index];
        this->data[parent_index] = temp;
        index = parent_index;
    }
    return index;
}

template<typename T>
template<int(*compare)(T, T)>
T Array<T>::heap_erase(size_t index) {
    T return_value = this->data[index];
    this->data[index] = this->pop();
    size_t child = index << 1;
    while (child < this->length()) {
        // Pick 2nd child when smaller, and exists
        if (child + 1 < this->length() && compare(this->data[child], this->data[child + 1]) > 0) child++;
        // Swap if child is smaller
        if (compare(this->data[index], this->data[child]) <= 0) break;
        T temp = this->data[index];
        this->data[index] = this->data[child];
        this->data[child] = temp;
        index = child;
        child <<= 1;
    }
    return return_value;
}



// Non-member functions:

template<typename T>
static bool array_expand(Array<T>* array, size_t size) {
    assert(array->byte_length <= array->byte_capacity);

    if (array->byte_length + size * sizeof(T) > array->byte_capacity) {
        size_t new_capacity = array->byte_capacity << 1;
        if (new_capacity < array->byte_length + size * sizeof(T))
            new_capacity = array->byte_length + size * sizeof(T);
        T *ptr = (T*)realloc(array->data, new_capacity);
        if (ptr == NULL) return false;
        array->data = ptr;
        array->byte_capacity = new_capacity;
    }
    return true;
}

//} // namespace game
