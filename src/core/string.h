#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "Core/Array.h"

// String_View is read only
// String can read and write
// String inherits String_View for implicit type cast to String_View
// Implicit cast to String_View allow both String and String_View as function arguments

// Read only
struct String {
    char* c_str;
    ssize_t len;

    char& operator[](ssize_t i) { assert(i >= 0 && i < len); return c_str[i]; };
    String operator=(const char* c_str) { this->c_str = (char*)c_str; this->len = strlen(c_str); return *this; }
};



bool resize(String* str, ssize_t size) {
    if (size == str->len) return true;
    void* ptr = realloc(str->c_str, size + 1);
    if (!ptr) return false;
    str->c_str = (char*)ptr;
    str->len = size;
    str->c_str[size] = '\0';
    return true;
}
void concat(String* out, String in) {
    ssize_t old_len = out->len;
    resize(out, out->len + in.len);
    memcpy(out->c_str + old_len, in.c_str, in.len);
}
void concat(String* out, char* in) {
    concat(out, {in, (ssize_t)strlen(in)});
}
void concat(String* out, char in) {
    resize(out, out->len + 1);
    out->c_str[out->len-1] = in;
    out->c_str[out->len] = '\0';
}
void concat(String* out, int in) {
    if (in < 0) {
        resize(out, out->len + 1);
        out->c_str[out->len] = '-';
    }
    int begin = out->len;
    do {
        resize(out, out->len + 1);
        out->c_str[out->len++] = '0' + in % 10;
        in /= 10;
    } while(in);
    out->c_str[out->len] = '\0';
    for (int i = begin; i < out->len; i++) {
        char temp = (*out)[i];
        out->c_str[i] = (*out)[out->len - begin + i];
        out->c_str[out->len - begin + i] = temp;
    }
}
void concat(String* out, bool in) {
    if (in)
        concat(out, {"true", 4});
    else
        concat(out, {"false", 5});
}

template<typename In, typename... Args>
void append(String* out, In in, Args... args) {
    concat(out, in);
    concat(out, args...);
}


String string_view(char* c_str) {
    return { c_str, (ssize_t)strlen(c_str) };
}
String string(String string_view) {
    String r = {};
    concat(&r, string_view);
    return r;
}
String string(char* c_str) {
    String r = {};
    concat(&r, c_str);
    return r;
}

ssize_t find(String a, String b) {
    char* str = strstr(a.c_str, b.c_str);
    if (str) return str - a.c_str;
    else return -1;
}

void replace(String* out, String a, String b) {
    String output = {};
    ssize_t old_index = 0;
    ssize_t new_index = find(*out, a);
    while (new_index >= old_index) {
        concat(&output, {out->c_str + old_index, new_index - old_index});
        concat(&output, b);
        new_index += a.len;
        old_index = new_index;
        new_index += find({out->c_str + new_index, out->len - new_index}, a);
    }
    concat(&output, {out->c_str + old_index, out->len - old_index});
    free(out->c_str);
    *out = output;
}

void sprint(String* out, char* format, ...) {
    va_list args;
    va_start(args, format);
    ssize_t old_len = out->len;
    ssize_t new_len = old_len + vsnprintf(out->c_str, -1, format, args);
    resize(out, new_len);
    vsprintf(out->c_str + old_len, format, args);
    va_end(args);
}

// power of 2 growth:
/*
bool reserve(String* str, ssize_t size) {
    assert(str->cap != -1);
    if (size <= str->cap) return true;
    ssize_t new_cap = cap-1;
    new_cap |= new_cap >> 1;
    new_cap |= new_cap >> 2;
    new_cap |= new_cap >> 4;
    new_cap |= new_cap >> 8;
    new_cap |= new_cap >> 16;
    new_cap |= new_cap >> 32;
    new_cap++;
    while(new_cap < size) new_cap <<= 1;
    void* ptr = realloc(str->c_str, new_cap);
    if (!ptr) return false;
    str->c_str = ptr;
    str->cap = new_cap;
    return true;
}

bool shrink_to_fit(String* str) {
    ssize_t new_cap = str->len+1;
    if (new_cap == str->cap) return true;
    void *ptr = realloc(str->c_str, new_cap);
    if (!ptr) return false;
    str->c_str = ptr;
    str->cap = new_cap;
    return true;
}*/

/*struct String {
    char* c_str;
    int len;
    int cap;

    char& operator[](int i) { assert(i >= 0 && i < len); return c_str[i]; }
};*/

/*String operator+(String string, int i) {
    string.c_str += i;
    string.len -= i;
    return string;
}

String operator-(String string, int i) { return string + -i; }
void operator+=(String& string, int i) { string = string + i; }
void operator-=(String& string, int i) { string = string - i; }
void operator++(String& string, int i) { string.c_str++; string.len--; }
void operator--(String& string, int i) { string.c_str--; string.len++; }*/
