// Copyrighted (c) John Emanuelsson - All rights reserved
// File created 2016-10-30

// TODO: Fix erase_node-bug (crash)

#include "core/quadtree.h"

static u32 quadtree_writer_create_node(Quadtree* quadtree_writer);


// Utility functions

int quadtree_pos_to_index(glm::u32vec3 pos) {
    return ((pos.x >> pos.z) & 1) + ((pos.y >> pos.z) & 1) * 2;
}

glm::u32vec3 quadtree_calc_child_pos(glm::u32vec3 parent_pos, int branch_direction) {
    assert(parent_pos.z < 32 && parent_pos.z >= 0);
    glm::u32vec2 direction = { branch_direction & 1, (branch_direction >> 1) & 1 };
    return glm::u32vec3(glm::u32vec2(2)*glm::u32vec2(parent_pos) + direction, parent_pos.z+1);
}


glm::u32vec3 quadtree_calc_parent_pos(glm::u32vec3 child_pos) {
    assert(child_pos.z <= 32 && child_pos.z > 0);
    return {child_pos.x/2, child_pos.y/2, child_pos.z-1};
}

glm::dvec2 quadtree_node_pos_to_dvec(glm::u32vec3 pos) {
    assert(pos.z <= 32);
    return glm::dvec2(pos) / glm::dvec2(float(1 << pos.z));
}

//

void Quadtree::init() {
    Array::init();
}

void Quadtree::destroy() {
    Array::destroy();
}

// Read

u32 Quadtree::find_node(glm::u32vec3 pos, u32 node) {
    assert(pos.z <= 32);
    assert(node+3 < this->length() || node == 0);
    if (pos.z == 0) return node;
    if (this->length() == 0) return -1;
    pos.z--;

    int index = quadtree_pos_to_index(pos);
    u32 child = this->data[node | index];
    if (child == 0) return -1;
    assert(node != child);
    return this->find_node(pos, child);
}

bool Quadtree::is_leaf(u32 node) {
    assert(node | 3 < this->length() || node == 0);
    if (this->length() == 0) return true;
    return !(this->data[node | 0] || this->data[node | 1] || this->data[node | 2] || this->data[node | 3]);
}

bool Quadtree::is_branch(u32 node) {
    assert(node | 3 < this->length() || node == 0);
    if (this->length() == 0) return false;
    return (this->data[node | 0] && this->data[node | 1] && this->data[node | 2] && this->data[node | 3]);
}

// Write

void Quadtree::free_nodes(Array<u32> nodes) {
    for (int i = 0; i < nodes.length(); i++) {
        this->data[nodes[i]] = this->removed_nodes_head;
        this->removed_nodes_head = nodes[i];
    }
}

void Quadtree::erase_children(u32 node, Array<u32>* erased_nodes, Array<u32>* erased_node_indices) {
    assert(node | 3 < this->length() && node >= 0 && node % 4 == 0);
    for (int i = 0; i < 4; ++i) {
        u32 child = this->data[node | i];
        if (child == 0) continue;
        assert(child != node);
        if (erased_nodes) erased_nodes->push(child);
        if (erased_node_indices) erased_node_indices->push(node | i);
        this->erase_children(child, erased_nodes, erased_node_indices);
        this->data[child] = this->removed_nodes_head;
        this->removed_nodes_head = child;
    }
    memset(&this->data[node], 0, 4 * sizeof(u32));
}

void Quadtree::erase_node(u32 node_index, Array<u32>* erased_nodes, Array<u32>* erased_node_indices) {
    assert(node_index | 3 < this->length() && node_index >= 0);
    u32 node = this->data[node_index];
    assert(node | 3 < this->length() && node > 0 && node % 4 == 0);
    assert(node != node_index);
    if (erased_nodes) erased_nodes->push(node);
    if (erased_node_indices) erased_node_indices->push(node_index);
    this->erase_children(node, erased_nodes, erased_node_indices);
    this->data[node] = this->removed_nodes_head;
    this->removed_nodes_head = node;
    this->data[node_index] = 0;
}

void Quadtree::insert_children(u32 node, Array<u32>* erased_nodes) {
    assert(node | 3 < this->length() && node >= 0 && node % 4 == 0);
    this->erase_children(node, erased_nodes);
    for (int i = 0; i < 4; ++i) {
        u32 child = quadtree_writer_create_node(this);
        assert(child != node);
        this->data[node | i] = child;
    }
}

u32 Quadtree::insert_node(glm::u32vec3 relative_pos, u32 base_node, Array<u32>* erased_nodes) {
    if (this->length() == 0) this->resize(4);
    assert(base_node | 3 < this->length() && base_node >= 0 && base_node % 4 == 0);
    assert(relative_pos.z <= 32 && relative_pos.z >= 0);

    if (relative_pos.z == 0) {
        this->erase_children(base_node, erased_nodes);
        return base_node;
    }
    relative_pos.z--;

    int index = quadtree_pos_to_index(relative_pos);
    if (this->data[base_node | index] == 0) {
        u32 child = quadtree_writer_create_node(this);
        assert(child != base_node);
        this->data[base_node | index] = child;
    }
    assert(base_node != this->data[base_node | index]);
    return this->insert_node(relative_pos, this->data[base_node | index], erased_nodes);
}

// static

static u32 quadtree_writer_create_node(Quadtree* quadtree_writer) {
    u32 node;
    if (quadtree_writer->removed_nodes_head) {
        node = quadtree_writer->removed_nodes_head;
        quadtree_writer->removed_nodes_head = quadtree_writer->data[node];
        memset(&quadtree_writer->data[node], 0, 4 * sizeof(u32));
    } else {
        node = quadtree_writer->length();
        quadtree_writer->grow(4);
    }
    assert(node | 3 < quadtree_writer->length() && node >= 0 && node % 4 == 0);
    return node;
}
