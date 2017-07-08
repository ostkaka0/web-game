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

