#pragma once

#include "core/common.h"

#include <stdarg.h>

template<typename T>
struct entity {
    static u32 id;
    static Array<T> data;
};
template<typename T>
u32 entity<T>::id = -1;
template<typename T>
Array<T> entity<T>::data = {0, 0, 0};

/*#define _ENTITY_MANAGER_INIT(COMPONENT) \
    entity<COMPONENT>::data = {0, 0, 0};\
    entity<COMPONENT>::id = __COUNTER__ - _entity_manager_first_count;\
    entity_callbacks[entity<COMPONENT>::id] = _##COMPONENT##_entity_callbacks;\
    assert(entity<COMPONENT>::id < 64);

#define _ENTITY_MANAGER_INIT(first, second, ...) \
    _ENTITY_MANAGER_INIT(first);\
    _ENTITY_MANAGER_INIT(second, __VA_ARGS__);*/

#define ENTITY_MANAGER_INIT(...) \
    memset(entity_callbacks, 0, 64 * sizeof(*entity_callbacks));\
    const u32 _entity_manager_first_count = __COUNTER__ + 1;\
    _ENTITY_MANAGER_INIT<__VA_ARGS__>();

static u32 _entity_manager_component_counter = 0;



Array<u64> entity_flags = {0, 0, 0};


struct Component {
    void deinit() {};
};

struct Entity_Callbacks {
    void(Component::*deinit)(u32 id);
};

Entity_Callbacks entity_callbacks[64];
struct { size_t size; } Component_info[64];
Array<u8>* Component_data[64];

template<typename First>
void _ENTITY_MANAGER_INIT() {
    entity<First>::data = {0, 0, 0};
    entity<First>::id = _entity_manager_component_counter++;
    Component_info[entity<First>::id] = { sizeof(First) };
    entity_callbacks[entity<First>::id].deinit = (void(Component::*)(u32))&First::deinit;//_##COMPONENT##_entity_callbacks;
    Component_data[entity<First>::id] = (Array<u8>*)(&entity<First>::data);
    assert(entity<First>::id < 64);
}

template<typename First, typename Second, typename... Rest>
void _ENTITY_MANAGER_INIT() {
    _ENTITY_MANAGER_INIT<First>();
    _ENTITY_MANAGER_INIT<Second, Rest...>();
}

void entity_manager_deinit();


u32 entity_create() {
    assert(0);
}

template<typename T>
T* entity_get(u32 id) {
    return &entity<T>::data[id];
}

Component* entity_get(u32 id, int component_id) {
    return (Component*)(*Component_data[component_id])[id * Component_info[component_id].size];
}

void entity_destroy(u32 id) {
    for (int i = 0; i < _entity_manager_component_counter; i++) {
        if ((entity_flags[id] >> i) & 1 && entity_callbacks[i].deinit) {
            Component* this_ptr = entity_get(id, i);
            (this_ptr->*entity_callbacks[id].deinit)(id);//entity_callbacks[id].deinit(NULL, id);
        }
    }
    assert(0);
}

template<typename T>
T entity_add(u32 id, T component) {
    if (entity<T>::data.length() <= id)
        entity<T>::data.resize(id + 1);

    entity<T>::data[id] = component;
    return component;
}

template<typename T>
void entity_remove(u32 id) {
    /*assert (entity_flags.length() > id)
    assert(entity_flags[id] & ((u64)1 << entity<T>::id));
    entity_flags[id] &= ~((u64)1 << entity<T>::id);*/
    entity_get<T>(id)->deinit(id);
}

