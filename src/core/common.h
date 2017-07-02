// Copyright (c) 2016 John Emanuelsson - All Rights Reserved

// This file contains utility code, should probably be moved to other files.

#pragma once

#include "core/macro.h"
#include "core/types.h"
#include "core/array.h"
#include "core/map.h"

#define SIZE_T_NULL ((size_t)-1)

//namespace game {

template<typename A, typename B>
struct Pair {
    union { A first, a; };
    union { B second, b; };
};

template<typename A, typename B>
static A operator< (Pair<A, B> a, Pair<A, B> b) {
    return a.first < b.first;
}

template<typename A, typename B>
static A operator- (Pair<A, B> a, Pair<A, B> b) {
    return a.first - b.first;
}

// not same as <, a-b<0 is not 100% same as a<b
template<typename T>
static auto less_than(T a, T b) {
    auto diff = a - b;
    if (diff < 0) return -1;
    else if (diff > 0) return 1;
    else return 0;
}

// not same as >, b-a<0 is not 100% same as a>b
template<typename T>
static auto more_than(T a, T b) {
    auto diff = b - a;
    if (diff < 0) return -1;
    else if (diff > 0) return 1;
    else return 0;
}

/*template<typename T, size_t size>
struct Static_Array {
    T data[size];

    T& operator[](size_t index) const { return this->data[index]; }
    operator T* () { return this->data; }
};*/

//} // namespace game
