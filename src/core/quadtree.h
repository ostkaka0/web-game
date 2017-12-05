// Copyrighted (c) John Emanuelsson - All rights reserved
// File created 2016-10-30

#pragma once

#include "core/common.h"

struct Quadtree : Array<u32> {
    Array<u32> parents;
    Array<glm::u32vec3> node_pos;
    u32 removed_nodes_head = 0; // Removed nodes accessed by linked list

    void init();
    void destroy();
    // Read
    u32 find_node(glm::u32vec3 pos, u32 node = 0);
    bool is_leaf(u32 node);
    bool is_branch(u32 node);
    u32 get_parent(u32 node);
    glm::u32vec3 get_pos_from_branch(u32 node);
    glm::u32vec3 get_pos_from_leaf(u32 node);
    glm::u32vec3 get_pos(u32 node);
    // Write
    void free_nodes(Array<u32> nodes);
    void erase_children(u32 node, Array<u32>* erased_nodes = NULL, Array<u32>* erased_node_indices = NULL);
    void erase_node(u32 node_index, Array<u32>* erased_nodes = NULL, Array<u32>* erased_node_indices = NULL);
    void insert_children(u32 node, Array<u32>* erased_nodes = NULL);
    u32 insert_node(u32 node_index, Array<u32>* erased_nodes = NULL);
};

int quadtree_pos_to_index(glm::u32vec3 pos);
glm::u32vec3 quadtree_calc_child_pos(glm::u32vec3 parent_pos, int branch_direction);
glm::u32vec3 quadtree_calc_parent_pos(glm::u32vec3 parent_pos);
glm::dvec2 quadtree_node_pos_to_dvec(glm::u32vec3 pos);

static u32 quadtree_writer_create_node(Quadtree* quadtree_writer);


// Utility functions

int quadtree_pos_to_index(glm::u32vec3 pos) {
	return ((pos.x >> pos.z) & 1) + ((pos.y >> pos.z) & 1) * 2;
}

glm::u32vec3 quadtree_calc_child_pos(glm::u32vec3 parent_pos, int branch_direction) {
	assert(parent_pos.z < 32 && parent_pos.z >= 0);
	glm::u32vec2 direction = { branch_direction & 1, (branch_direction >> 1) & 1 };
	return glm::u32vec3(glm::u32vec2(2)*glm::u32vec2(parent_pos) + direction, parent_pos.z + 1);
}


glm::u32vec3 quadtree_calc_parent_pos(glm::u32vec3 child_pos) {
	assert(child_pos.z <= 32 && child_pos.z > 0);
	return { child_pos.x / 2, child_pos.y / 2, child_pos.z - 1 };
}

glm::dvec2 quadtree_node_pos_to_dvec(glm::u32vec3 pos) {
	assert(pos.z <= 32);
	return glm::dvec2(pos) / glm::dvec2(float(1 << pos.z));
}

//

void Quadtree::init() {
	memset(this, 0, sizeof(*this));
}

void Quadtree::destroy() {
	Array::destroy();
	this->parents.destroy();
	this->node_pos.destroy();
}

// Read

u32 Quadtree::find_node(glm::u32vec3 pos, u32 node) {
	assert(pos.z <= 32);
	assert(node + 3 < this->length() || node == 0);
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
	if (this->length() <= 4) return true;
	assert(node | 3 < this->length());
	return !(this->data[node | 0] || this->data[node | 1] || this->data[node | 2] || this->data[node | 3]);
}

bool Quadtree::is_branch(u32 node) {
	if (this->length() <= 4) return false;
	assert(node | 3 < this->length());
	return (this->data[node | 0] && this->data[node | 1] && this->data[node | 2] && this->data[node | 3]);
}

u32 Quadtree::get_parent(u32 node) {
	return this->parents[node >> 2];
}

glm::u32vec3 Quadtree::get_pos_from_branch(u32 node) {
	return this->node_pos[node >> 2];
}

glm::u32vec3 Quadtree::get_pos_from_leaf(u32 node) {
	if (node >> 2 == 0) return { 0, 0, 0 };
	u32 parent = this->parents[node >> 2];
	return quadtree_calc_child_pos(this->node_pos[parent >> 2], parent);
}

glm::u32vec3 Quadtree::get_pos(u32 node) {
	if (is_leaf(node))
		return get_pos_from_leaf(node);
	else
		return get_pos_from_branch(node);
}

// Write

void Quadtree::free_nodes(Array<u32> nodes) {
	for (int i = 0; i < nodes.length(); i++) {
		this->data[nodes[i]] = this->removed_nodes_head;
		this->removed_nodes_head = nodes[i];
	}
}

void Quadtree::erase_children(u32 node, Array<u32>* erased_nodes, Array<u32>* erased_node_indices) {
	node &= ~3;
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
	node &= ~3;
	assert(node | 3 < this->length() && node >= 0 && node % 4 == 0);
	this->erase_children(node, erased_nodes);
	for (int i = 0; i < 4; ++i) {
		u32 child = quadtree_writer_create_node(this);
		assert(child != node);
		this->data[node | i] = child;     // node-child
		this->parents[child >> 2] = node | i; // node-parent
	}
	if (this->node_pos.length() <= node >> 2) this->node_pos.resize((node >> 2) + 1);
	this->node_pos[node >> 2] = this->get_pos_from_leaf(node); // parent-pos
}

u32 Quadtree::insert_node(u32 node_index, Array<u32>* erased_node_indexs) {
	if (this->length() == 0) { this->resize(4); this->parents.resize(4); }
	assert(node_index | 3 < this->length());

	u32 child = this->data[node_index];
	if (child == 0) {
		child = quadtree_writer_create_node(this);
		assert(child != node_index);
		this->data[node_index] = child;
		this->parents[child >> 2] = node_index;
		if (this->node_pos.length() <= node_index >> 2) this->node_pos.resize((node_index >> 2) + 1);
		this->node_pos[node_index >> 2] = this->get_pos_from_leaf(node_index);
	}
	assert(node_index >> 2 != this->data[node_index] >> 2);
	return child;
}

// static

static u32 quadtree_writer_create_node(Quadtree* quadtree_writer) {
	u32 node;
	if (quadtree_writer->removed_nodes_head) {
		node = quadtree_writer->removed_nodes_head;
		quadtree_writer->removed_nodes_head = quadtree_writer->data[node];
		memset(&quadtree_writer->data[node], 0, 4 * sizeof(u32));
	}
	else {
		node = quadtree_writer->length();
		quadtree_writer->grow(4);
		quadtree_writer->parents.grow(1);
	}
	assert(node | 3 < quadtree_writer->length() && node >= 0 && node % 4 == 0);
	return node;
}


