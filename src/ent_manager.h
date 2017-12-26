#pragma once

/*  Terminology:
        ent - entity, usually just the ent-id
        ent    - entity component
*/

#include "core/common.h"

#include <stdarg.h>

struct Ent {
    void deinit(u32 id) {}
    static void ent_init() {}
    static void ent_deinit() {}
};

// Ent info from type by template
template<typename T>
struct g_ent_info {
    static u32 id;
    static Array<T> data;
};
template<typename T>
u32 g_ent_info<T>::id = -1;
template<typename T>
Array<T> g_ent_info<T>::data = { 0, 0, 0 };

// Ent info from id
struct {
    void(Ent::*deinit)(u32 id);
    void(*ent_deinit)();
} g_ent_callbacks[64];
size_t g_ent_sizes[64];
Array<u8>* g_ent_data[64];
int g_num_components;

Array<u64> g_ent_flags;
u64 g_ent_free_head; // Keep track of removed entities, stored in ent_flags as linked list of indices


template<typename C>
void ent_manager_init_ent() {
    g_ent_info<C>::data = { 0, 0, 0 };
    g_ent_info<C>::id = g_num_components++;
    g_ent_sizes[g_ent_info<C>::id] = { sizeof(C) };
    g_ent_callbacks[g_ent_info<C>::id].deinit = (void(Ent::*)(u32))(&C::deinit);
    g_ent_callbacks[g_ent_info<C>::id].ent_deinit = (void(*)())&C::ent_deinit;
    g_ent_info<C>::data.init();
    g_ent_data[g_ent_info<C>::id] = (Array<u8>*)(&g_ent_info<C>::data);
    assert(g_ent_info<C>::id < 64);

    C::ent_init();
}

template<typename First, typename Second, typename... Rest>
void ent_manager_init_ent() {
    ent_manager_init_ent<First>();
    ent_manager_init_ent<Second, Rest...>();
}

template<typename... Args>
void ent_manager_init() {
    g_num_components = 0;
    g_ent_flags.init();
    g_ent_free_head = -1;

    ent_manager_init_ent<Args...>();
}

void ent_manager_deinit() {
    g_ent_flags.destroy();
    for (int i = 0; i < g_num_components; i++) {
        g_ent_callbacks[i].ent_deinit();
        g_ent_data[i]->destroy();
    }
}

// Get ent-component by component-type
#define ent_get(id, T) (&g_ent_info<Ent_##T>::data[id])
#define ent_get2(id, T) (&g_ent_info<T>::data[id])

// Get ent-component by component-id
void* ent_get_by_id(u32 id, int ent_id) {
    return (void*)(*g_ent_data[ent_id])[id * g_ent_sizes[ent_id]];
}

u32 ent_create() {
    u32 id;
    if (g_ent_free_head == -1) {
        id = g_ent_flags.length();
        g_ent_flags.push(0);
    }
    else {
        // Pop linked list
        id = (u32)g_ent_free_head;
        g_ent_free_head = g_ent_flags[g_ent_free_head];
    }
    g_ent_flags[id] = 0;
    return id;
}

void ent_destroy(u32 id) {
    for (int i = 0; i < g_num_components; i++) {
        if ((g_ent_flags[id] >> i) & 1 && g_ent_callbacks[i].deinit) {
            Ent* this_ptr = (Ent*)ent_get_by_id(id, i);
            (this_ptr->*g_ent_callbacks[id].deinit)(id);//ent_callbacks[id].deinit(NULL, id);
        }
    }
    // Push linked list
    g_ent_flags[id] = g_ent_free_head;
    g_ent_free_head = (u64)id;
}

template<typename T>
T ent_add(u32 id, T component) {
    assert(g_ent_flags.length() > id);
    assert((g_ent_flags[id] & ((u64)1 << g_ent_info<T>::id)) == 0);

    g_ent_flags[id] |= ((u64)1 << g_ent_info<T>::id);

    component.init(id);

    if (g_ent_info<T>::data.length() <= id)
        g_ent_info<T>::data.resize(id + 1);
    g_ent_info<T>::data[id] = component;

    return component;
}

template<typename T>
void ent_remove(u32 id) {
    assert(g_ent_flags.length() > id);
    assert(g_ent_flags[id] & ((u64)1 << g_ent_info<T>::id));
    g_ent_flags[id] &= ~((u64)1 << g_ent_info<T>::id);
    ent_get2(id, T)->deinit(id);
}

