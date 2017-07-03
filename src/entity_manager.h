#pragma once

#include "core/common.h"

#include <stdarg.h>

// Component info from type by template
template<typename T>
struct g_component_info {
    static u32 id;
    static Array<T> data;
};
template<typename T>
u32 g_component_info<T>::id = -1;
template<typename T>
Array<T> g_component_info<T>::data = {0, 0, 0};

// Component info from id
struct _Component { };
struct {
    void(_Component::*deinit)(u32 id);
} g_component_callbacks[64];
size_t g_component_sizes[64];
Array<u8>* g_component_data[64];
int g_num_components;

Array<u64> g_entity_flags;
u64 g_entity_free_head; // Keep track of removed entities, stored in entity_flags as linked list of indices


template<typename Component>
void _entity_manager_init() {
    g_component_info<Component>::data = {0, 0, 0};
    g_component_info<Component>::id = g_num_components++;
    g_component_sizes[g_component_info<Component>::id] = { sizeof(Component) };
    g_component_callbacks[g_component_info<Component>::id].deinit = (void(_Component::*)(u32))&Component::deinit;//_##COMPONENT##_entity_callbacks;
    g_component_info<Component>::data.init();
    g_component_data[g_component_info<Component>::id] = (Array<u8>*)(&g_component_info<Component>::data);
    assert(g_component_info<Component>::id < 64);
}

template<typename First, typename Second, typename... Rest>
void _entity_manager_init() {
    _entity_manager_init<First>();
    _entity_manager_init<Second, Rest...>();
}

template<typename... Args>
void entity_manager_init() {
    g_num_components = 0;
    g_entity_flags.init();
    g_entity_free_head = -1;

    _entity_manager_init<Args...>();
}

void entity_manager_deinit() {
    g_entity_flags.destroy();
    for (int i = 0; i < g_num_components; i++) {
        g_component_data[i]->destroy();
    }
}

// Get component by component-type
template<typename T>
T* entity_get(u32 id) {
    return &g_component_info<T>::data[id];
}

// Get component by component-id
void* entity_get(u32 id, int component_id) {
    return (_Component*)(*g_component_data[component_id])[id * g_component_sizes[component_id]];
}

u32 entity_create() {
    u32 id;
    if (g_entity_free_head == -1) {
        id = g_entity_flags.length();
        g_entity_flags.push(0);
    } else {
        // Pop linked list
        id = (u32)g_entity_free_head;
        g_entity_free_head = g_entity_flags[g_entity_free_head];
    }
    g_entity_flags[id] = 0;
    return id;
}

void entity_destroy(u32 id) {
    for (int i = 0; i < g_num_components; i++) {
        if ((g_entity_flags[id] >> i) & 1 && g_component_callbacks[i].deinit) {
            _Component* this_ptr = (_Component*)entity_get(id, i);
            (this_ptr->*g_component_callbacks[id].deinit)(id);//entity_callbacks[id].deinit(NULL, id);
        }
    }
    // Push linked list
    g_entity_flags[id] = g_entity_free_head;
    g_entity_free_head = (u64)id;
}

template<typename T>
T component_add(u32 id, T component) {
    assert(g_entity_flags.length() > id);
    assert((g_entity_flags[id] & ((u64)1 << g_component_info<T>::id)) == 0);

    g_entity_flags[id] |= ((u64)1 << g_component_info<T>::id);

    if (g_component_info<T>::data.length() <= id)
        g_component_info<T>::data.resize(id + 1);
    g_component_info<T>::data[id] = component;
    return component;
}

template<typename T>
void component_remove(u32 id) {
    assert(g_entity_flags.length() > id);
    assert(g_entity_flags[id] & ((u64)1 << g_component_info<T>::id));
    g_entity_flags[id] &= ~((u64)1 << g_component_info<T>::id);
    entity_get<T>(id)->deinit(id);
}

