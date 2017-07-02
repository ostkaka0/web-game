// Copyright (c) 2016 John Emanuelsson - All Rights Reserved
// File created 2016

#pragma once

#include "core/array.h"
#include "core/hash.h"

//namespace game {

template<typename Key>
struct Set {
    Array<int> freeIndices;
    int* buckets;
    Key* keys;
    int numBuckets;
    int length;
    int capacity;

    void init();
    void destroy();
    bool has_key(Key key);
    bool insert(Key key);
    bool erase(Key key);
    //template<typename Function>
    //void foreach(Function function);
};

template<typename Key, typename Value>
struct Map : Set<Key> {
    Value* values;   

    void init();
    void destroy();
    Value* find(Key key);
    bool insert(Key key) = delete;
    bool insert(Key key, Value value);
    //template<typename Function>
    //void foreach(Function function);
};


template<typename Key>
static int* set_find_bucket(Set<Key>* set, const Key key);
template<typename Key>
static int* set_find_free_bucket(Set<Key>* set, const Key key);
template<typename Key>
static int set_bucket_remove(Set<Key>* set, int* bucket);
template<typename Key>
static int set_expand(Set<Key>* set);
template<typename Key, typename Value>
static int map_expand(Map<Key, Value>* map);

template<typename Key>
void Set<Key>::init() {
    memset(this, 0, sizeof(*this));
}

template<typename Key>
void Set<Key>::destroy() {
    this->freeIndices.destroy();
    free(this->buckets);
    free(this->keys);
}

template<typename Key>
bool Set<Key>::has_key(Key key) {
    if (!this->buckets) return false;
    return set_find_bucket(this, key);
}

template<typename Key>
bool Set<Key>::insert(Key key) {
    if (this->freeIndices.length() == 0) {
        if (set_expand(this))
            return false;
    }

    int* bucket = set_find_free_bucket(this, key);//int index = length;
    if (*bucket == -1) {
        if (this->freeIndices.length() == 0) {
            *bucket = this->length;
            this->length++;
        }
        else
            *bucket = this->freeIndices.pop();
    }
    this->keys[*bucket] = key;
    return true;
}

template<typename Key>
bool Set<Key>::erase(Key key) {
    if (!this->buckets) return true;
    int* bucket = set_find_bucket(this, key);
    if (!bucket) return false;
    this->freeIndices.push(*bucket);
    return (set_bucket_remove(this, bucket) == 0);
}


template<typename Key, typename Value>
void Map<Key, Value>::init() {
    memset(this, 0, sizeof(*this));
}

template<typename Key, typename Value>
void Map<Key, Value>::destroy() {
    Set<Key>::destroy();
    free(this->values);
}

template<typename Key, typename Value>
Value* Map<Key, Value>::find(Key key) {
    if (!this->buckets) return NULL;
    int* bucket = set_find_bucket(this, key);
    if (!bucket)
        return NULL;
    return &this->values[*bucket];
}

template<typename Key, typename Value>
bool Map<Key, Value>::insert(Key key, Value value) {
    if (this->freeIndices.length() == 0) {
        if (map_expand(this))
            return false;
    }

    int* bucket = set_find_free_bucket(this, key);//int index = length;
    if (*bucket == -1) {
        if (this->freeIndices.length() == 0) {
            *bucket = this->length;
            this->length++;
        }
        else
            *bucket = this->freeIndices.pop();
    }
    this->keys[*bucket] = key;
    this->values[*bucket] = value;
    return true;
}

template<typename Key>
static int* set_find_bucket(Set<Key>* set, const Key key) {
    int bucketId = (size_t)hash(key) & (set->numBuckets-1);
    int* bucket = &set->buckets[bucketId];
    while (true) {
        if (*bucket == -1)
            return NULL;
        if (set->keys[*bucket] == key)
            return bucket;

        bucketId = (bucketId + 1) % set->numBuckets;
        bucket = &set->buckets[bucketId];
    }
}

template<typename Key>
static int* set_find_free_bucket(Set<Key>* set, const Key key) {
    int bucketId = (size_t)hash(key) & (set->numBuckets - 1);
    int* bucket = &set->buckets[bucketId];
    while (true) {
        if (*bucket == -1)
            return bucket;
        if (set->keys[*bucket] == key)
            return bucket;

        bucketId = (bucketId + 1) % set->numBuckets;
        bucket = &set->buckets[bucketId];
    }
}

template<typename Key>
static int set_bucket_remove(Set<Key>* set, int* bucket) {
    *bucket = -1;
    int nextBucketIndex = (int)(bucket - (set->buckets) + 1) % set->numBuckets;
    //if (&bucket[1] >= &(*buckets)[*length]) return 0;
    if (set->buckets[nextBucketIndex] == -1) return 0;

    int index = set->buckets[nextBucketIndex];
    Key key = set->keys[index];
    if ((size_t)hash(key) & ((size_t)set->numBuckets - 1) == nextBucketIndex) return 0;

    int err = set_bucket_remove(set, &set->buckets[nextBucketIndex]);
    if (err) return err;

    int* newBucket = set_find_free_bucket(set, key);
    if (!newBucket) return -1;
    *newBucket = index;
    return 0;
}

template<typename Key>
static int set_expand(Set<Key>* set) {
    if (set->length + 1 <= set->capacity)
        return 0;

    set->capacity = (set->capacity == 0) ? 1 : (set->capacity) << 1;
    set->numBuckets = (set->numBuckets == 0) ? 2 : (set->numBuckets) << 1;
    set->keys = (Key*)realloc(set->keys, set->capacity * sizeof(Key));
    set->buckets = (int*)realloc(set->buckets, set->numBuckets * sizeof(int));
    if (!set->keys || !set->buckets) {
        free(set->keys);
        free(set->buckets);
        set->keys = NULL;
        set->buckets = NULL;
        return -1;
    }
    memset(set->buckets, -1, set->numBuckets * sizeof(int));
    for (int keyIndex = 0; keyIndex < set->length; ++keyIndex)
        *set_find_free_bucket(set, set->keys[keyIndex]) = keyIndex;

    return 0;
}

template<typename Key, typename Value>
static int map_expand(Map<Key, Value>* map) {
    if (map->length + 1 <= map->capacity)
        return 0;

    map->capacity = (map->capacity == 0) ? 1 : (map->capacity) << 1;
    map->numBuckets = (map->numBuckets == 0) ? 2 : (map->numBuckets) << 1;
    map->keys = (Key*)realloc(map->keys, map->capacity * sizeof(Key));
    map->values = (Value*)realloc(map->values, map->capacity * sizeof(Value));
    map->buckets = (int*)realloc(map->buckets, map->numBuckets * sizeof(int));
    if (!map->keys || !map->values ||!map->buckets) {
        free(map->keys);
        free(map->values);
        free(map->buckets);
        map->keys = NULL;
        map->values = NULL;
        map->buckets = NULL;
        return -1;
    }
    memset(map->buckets, -1, map->numBuckets * sizeof(int));
    for (int keyIndex = 0; keyIndex < map->length; ++keyIndex)
        *set_find_free_bucket(map, map->keys[keyIndex]) = keyIndex; //_set_bucket(set, *keys[keyIndex]);

    return 0;
}

//} // namespace game

///////////////////////////////////////////////////////////////////////
/*#pragma once

#include <cstring>

#include "Engine/Core/Types.h"
#include "Engine/Core/Array.h"
#include "Engine/Core/Macro.h"

#include "Engine/Core/Hash.h"

//#define SET_ARGS Array<int>* freeIndices, int** buckets, Key** keys, int* numBuckets, int* length, int* capacity
//#define MAP_ARGS Array<int>* freeIndices, int** buckets, Key** keys, Value** values, int* numBuckets, int* length, int* capacity
#define SET_UNPACK(set) (Array<int>*)&(set)->freeIndices ,(int**)&(set)->buckets, /*(Key**)* /&(set)->keys, (int*)&(set)->numBuckets, (int*)&(set)->length, (int*)&(set)->capacity
#define MAP_UNPACK(map) (Array<int>*)&(map)->freeIndices ,(int**)&(map)->buckets, /*(Key**)* /&(map)->keys, /*(Value**)* /&(map)->values, (int*)&(map)->numBuckets, (int*)&(map)->length, (int*)&(map)->capacity

#define set_init(set) memset((set), 0, sizeof(*(set)))
#define set_destroy(set) (set)->freeIndices.destroy(); free((set)->buckets); free((set)->keys)
#define map_init(map) memset((map), 0, sizeof(*(map)))
#define map_destroy(map) (map)->freeIndices.destroy(); free((map)->buckets); free((map)->keys); free((map)->values)
#define set_haskey(set, key) _set_haskey(SET_UNPACK(set), key)
#define map_haskey(set, key) _set_haskey(SET_UNPACK(set), key)
#define set_find_bucket(set) _set_find_bucket(SET_UNPACK(set))
#define map_find(map, key) _map_find(MAP_UNPACK(map), key)
#define set_insert(set, key) _set_insert(SET_UNPACK(set), key)
#define map_insert(map, key, value) _map_insert(MAP_UNPACK(map), key, value)
#define set_erase(set, key) _set_erase(SET_UNPACK(set), key)
#define map_erase(map, key) _map_erase(MAP_UNPACK(map), key)

//#define set_first_key(set) (set)->keys[0]
//#define set_last_key(set) (set)->keys[(set)->length-1]
//#define map_first_value(map) (map)->keys[0]
//#define map_flast_value(map) (map)->keys[(map)->length-1]

#define set_next_it(set, it) _set_next_it(SET_UNPACK(set), it)
#define set_previous_it(set, it) _set_previous_it(SET_UNPACK(set), it)
#define map_next_it(map, it) _map_next_it(MAP_UNPACK(map), it)
#define map_previous_it(map, it) _map_previous_it(MAP_UNPACK(map), it)
#define set_first_it(set) set_next_it(set, { })
#define set_last_it(set) set_previous_it(set, { nullptr, (set)->length, (set)->freeIndices.length()-1 })
#define map_first_it(map) map_next_it(map, { })
#define map_last_it(map) map_previous_it(map, { nullptr, (map)->length, (map)->freeIndices.length()-1 })

//namespace game {

template<typename Key>
struct SetIterator {
	Key* key = nullptr;
	int index = -1;
	int nextFreeIndex = 0;
};

template<typename Key, typename Value>
struct MapIterator {
	Key* key = nullptr;
	Value* value = nullptr;
	int index = -1;
	int nextFreeIndex = 0;
};

// todo: setDestroy, mapDestroy

template<typename Key>
static int* _set_find_bucket(Array<int>* freeIndices, int** buckets, Key** keys, int* numBuckets, int* length, int* capacity, const Key key) {
	int bucketId = (size_t)hash(key) & ((size_t)*numBuckets-1);
	int* bucket = &(*buckets)[bucketId];
	while (true) {
		if (*bucket == -1)
			return NULL;
		if ((*keys)[*bucket] == key)
			return bucket;

		bucketId = (bucketId + 1) % *numBuckets;
		bucket = &(*buckets)[bucketId];
	}
}

template<typename Key>
static int* _set_find_free_bucket(Array<int>* freeIndices, int** buckets, Key** keys, int* numBuckets, int* length, int* capacity, const Key key) {
	int bucketId = (size_t)hash(key) & ((size_t)*numBuckets - 1);
	int* bucket = &(*buckets)[bucketId];
	while (true) {
		if (*bucket == -1)
			return bucket;
		if ((*keys)[*bucket] == key)
			return bucket;

		bucketId = (bucketId + 1) % *numBuckets;
		bucket = &(*buckets)[bucketId];
	}
}

template<typename Key>
static int _set_bucket_remove(Array<int>* freeIndices, int** buckets, Key** keys, int* numBuckets, int* length, int* capacity, int* bucket) {
	*bucket = -1;
	int nextBucketIndex = ((int)((int*)bucket - (int*)*buckets) + 1) % (*numBuckets);
	//if (&bucket[1] >= &(*buckets)[*length]) return 0;
	if ((*buckets)[nextBucketIndex] == -1) return 0;

	int index = (*buckets)[nextBucketIndex];
	Key key = (*keys)[index];
	if ((size_t)hash(key) & ((size_t)*numBuckets - 1) == nextBucketIndex) return 0;

	int err = _set_bucket_remove(freeIndices, buckets, keys, numBuckets, length, capacity, &(*buckets)[nextBucketIndex]);
	if (err) return err;

	int* newBucket = _set_find_free_bucket(freeIndices, buckets, keys, numBuckets, length, capacity, key);
	if (!newBucket) return -1;
	*newBucket = index;
	return 0;
}

template<typename Key>
static bool _set_haskey(Array<int>* freeIndices, int** buckets, Key** keys, int* numBuckets, int* length, int* capacity, const Key key) {
	if (!*buckets) return false;
	return _set_find_bucket(freeIndices, buckets, keys, numBuckets, length, capacity, key);
}

template<typename Key, typename Value>
static Value* _map_find(Array<int>* freeIndices, int** buckets, Key** keys, Value** values, int* numBuckets, int* length, int* capacity, const Key key) {
	if (!*buckets) return NULL;
	int* bucket = _set_find_bucket(freeIndices, buckets, keys, numBuckets, length, capacity, key);
	if (!bucket)
		return NULL;

	return &(*values)[*bucket];
}

template<typename Key>
static int _set_expand(Array<int>* freeIndices, int** buckets, Key** keys, int* numBuckets, int* length, int* capacity) {
	if (*length + 1 <= *capacity)
		return 0;

	*capacity = (*capacity == 0) ? 1 : (*capacity) << 1;
	*numBuckets = (*numBuckets == 0) ? 2 : (*numBuckets) << 1;
	*keys = (Key*)realloc(*keys, *capacity * sizeof(Key));
	*buckets = (int*)realloc(*buckets, *numBuckets * sizeof(int));
	if (!*keys || !*buckets) {
		free(*keys);
		free(*buckets);
		*keys = NULL;
		*buckets = NULL;
		return -1;
	}
    memset(*buckets, -1, *numBuckets * sizeof(int));
	for (int keyIndex = 0; keyIndex < *length; ++keyIndex)
		*_set_find_free_bucket(freeIndices, buckets, keys, numBuckets, length, capacity, (*keys)[keyIndex]) = keyIndex;

	return 0;
}

template<typename Key, typename Value>
static int _map_expand(Array<int>* freeIndices, int** buckets, Key** keys, Value** values, int* numBuckets, int* length, int* capacity) {
	if (*length + 1 <= *capacity)
		return 0;

	*capacity = (*capacity == 0) ? 1 : (*capacity) << 1;
	*numBuckets = (*numBuckets == 0) ? 2 : (*numBuckets) << 1;
	*keys = (Key*)realloc(*keys, *capacity * sizeof(Key));
	*values = (Value*)realloc(*values, *capacity * sizeof(Value));
	*buckets = (int*)realloc(*buckets, *numBuckets * sizeof(int));
	if (!*keys || !*values ||!*buckets) {
		free(*keys);
		free(*values);
		free(*buckets);
		*keys = NULL;
		*values = NULL;
		*buckets = NULL;
		return -1;
	}
	memset(*buckets, -1, *numBuckets * sizeof(int));
	for (int keyIndex = 0; keyIndex < *length; ++keyIndex)
		*_set_find_free_bucket(freeIndices, buckets, keys, numBuckets, length, capacity, (*keys)[keyIndex]) = keyIndex; //_set_bucket(set, *keys[keyIndex]);

	return 0;
}

template<typename Key>
static int _set_insert(Array<int>* freeIndices, int** buckets, Key** keys, int* numBuckets, int* length, int* capacity, const Key key) {
    if (freeIndices->length() == 0) {
		if (_set_expand(freeIndices, buckets, keys, numBuckets, length, capacity))
			return -1;
	}

	int* bucket = _set_find_free_bucket(freeIndices, buckets, keys, numBuckets, length, capacity, key);//int index = length;
	if (*bucket == -1) {
        if (freeIndices->length() == 0) {
			*bucket = *length;
			(*length)++;
		}
		else
            *bucket = freeIndices->pop();
	}
	(*keys)[*bucket] = key;
	return 0;
}

template<typename Key, typename Value>
static int _map_insert(Array<int>* freeIndices, int** buckets, Key** keys, Value** values, int* numBuckets, int* length, int* capacity, const Key key, const Value value) {
    if (freeIndices->length() == 0) {
		if (_map_expand(freeIndices, buckets, keys, values, numBuckets, length, capacity))
			return -1;
	}

	int* bucket = _set_find_free_bucket(freeIndices, buckets, keys, numBuckets, length, capacity, key);//int index = length;
	if (*bucket == -1) {
        if (freeIndices->length() == 0) {
			*bucket = *length;
			(*length)++;
		}
		else
            *bucket = freeIndices->pop();
	}
	(*keys)[*bucket] = key;
	(*values)[*bucket] = value;
	return 0;
}

template<typename Key>
static int _set_erase(Array<int>* freeIndices, int** buckets, Key** keys, int* numBuckets, int* length, int* capacity, const Key key) {
	if (!*buckets) return 0;
	int* bucket = _set_find_bucket(freeIndices, buckets, keys, numBuckets, length, capacity, key);
	if (!bucket) return -1;
    freeIndices->push(*bucket);
	return _set_bucket_remove(freeIndices, buckets, keys, numBuckets, length, capacity, bucket);
}

template<typename Key, typename Value>
static int _map_erase(Array<int>* freeIndices, int** buckets, Key** keys, Value** values, int* numBuckets, int* length, int* capacity, const Key key) {
	if (!*buckets) return 0;
	int* bucket = _set_find_bucket(freeIndices, buckets, keys, numBuckets, length, capacity, key);
	if (!bucket) return -1;
    freeIndices->push(*bucket);
	return _set_bucket_remove(freeIndices, buckets, keys, numBuckets, length, capacity, bucket);
}

template<typename Key>
static SetIterator<Key> _set_next_it(Array<int>* freeIndices, int** buckets, Key** keys, int* numBuckets, int* length, int* capacity, SetIterator<Key> iterator) {
	iterator.index++;
    if (iterator.nextFreeIndex < freeIndices->length()) {
		while (freeIndices->data[iterator.nextFreeIndex] == iterator.index) {
			iterator.nextFreeIndex++;
			iterator.index++;
            if (iterator.nextFreeIndex >= freeIndices->length()) {
				iterator.nextFreeIndex--;
				break;
			}
		}
	}
	if (iterator.index < *length)
		iterator.key =  &(*keys)[iterator.index];
	else
		iterator.key = NULL;
	return iterator;
}

template<typename Key>
static SetIterator<Key> _set_previous_it(Array<int>* freeIndices, int** buckets, Key** keys, int* numBuckets, int* length, int* capacity, SetIterator<Key> iterator) {
	iterator.index--;
	if (iterator.nextFreeIndex >= 0) {
		while (freeIndices->data[iterator.nextFreeIndex] == iterator.index) {
			iterator.nextFreeIndex--;
			iterator.index--;
			if (iterator.nextFreeIndex <= 0) {
				iterator.nextFreeIndex++;
				break;
			}
		}
	}
	if (iterator.index >= 0)
		iterator.key = &(*keys)[iterator.index];
	else
		iterator.key = NULL;
	return iterator;
}

template<typename Key, typename Value>
static MapIterator<Key, Value> _map_next_it(Array<int>* freeIndices, int** buckets, Key** keys, Value** values, int* numBuckets, int* length, int* capacity, MapIterator<Key, Value> iterator) {
	iterator.index++;
    if (iterator.nextFreeIndex < freeIndices->length()) {
		while (freeIndices->data[iterator.nextFreeIndex] == iterator.index) {
			iterator.nextFreeIndex++;
			iterator.index++;
            if (iterator.nextFreeIndex >= freeIndices->length()) {
				iterator.nextFreeIndex--;
				break;
			}
		}
	}
	if (iterator.index < *length) {
		iterator.key = &(*keys)[iterator.index];
		iterator.value = &(*values)[iterator.index];
	} else {
		iterator.key = NULL;
		iterator.value = NULL;
	}
	return iterator;
}

template<typename Key, typename Value>
static MapIterator<Key, Value> _map_previous_it(Array<int>* freeIndices, int** buckets, Key** keys, Value** values, int* numBuckets, int* length, int* capacity, MapIterator<Key, Value> iterator) {
	iterator.index--;
	if (iterator.nextFreeIndex >= 0) {
		while (freeIndices->data[iterator.nextFreeIndex] == iterator.index) {
			iterator.nextFreeIndex--;
			iterator.index--;
			if (iterator.nextFreeIndex <= 0) {
				iterator.nextFreeIndex++;
				break;
			}
		}
	}
	if (iterator.index >= 0) {
		iterator.key = &(*keys)[iterator.index];
		iterator.value = &(*values)[iterator.index];
	} else {
		iterator.key = NULL;
		iterator.value = NULL;
	}
	return iterator;
}

//} // namespace game*/
