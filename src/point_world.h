#pragma once

#include "core/common.h"
#include "core/quadtree.h"

#define POINT_WORLD_MAX_NODE_POINTS 4

// TODO: Multiple quadtree-chunks
struct Point_World {
    double size;

    Quadtree          quadtree;
    Array<Array<u32>> quadtree_points; // (node_id >> 2) -> node_array    idk: // = {{0,0,0}});

    Array<u32>        point_node;      // point_id -> node_id
    Array<glm::dvec2> point_pos;       // point_id -> point_pos
    Array<double>     point_radius;    // point_id -> point_radius
    u32               point_free_head; // Linked list of free points, contained by point_node


    void init(double size = 0);
    void deinit();

    u32 add(glm::dvec2 pos, double radius);
    void remove(u32 point_id);
    void move_point(u32 point_id, glm::dvec2 pos, double radius);
    void find_in_radius(Array<u32>* points, glm::dvec2 pos, double radius, u32 node = 0, glm::u32vec3 node_pos = {0, 0, 0});

    u32 find_node_by_pos(glm::u32vec3* node_pos, u32 node, glm::dvec2 pos, double radius);
    void insert_to_tree(glm::dvec2, double radius, u32 point_id);
    void merge_parent(u32 child_to_merge);
    void erase_from_tree(u32 point_id, glm::dvec2 pos, double radius);
};

void Point_World::init(double size ) {
    memset(this, 0, sizeof(*this));
    this->size = size;
}

void Point_World::deinit() {
    this->quadtree.destroy();
    this->quadtree_points.destroy();
    this->point_node.destroy();
    this->point_pos.destroy();
    this->point_radius.destroy();
}

u32 Point_World::add(glm::dvec2 pos, double radius) {
    u32 point_id;
    if (this->point_free_head >= this->point_node.length()) {
        point_id = this->point_node.length();
        this->point_node.grow(1);
        this->point_pos.grow(1);
        this->point_radius.grow(1);
        this->point_free_head = this->point_node.length();
    } else {
        point_id = this->point_free_head;
        this->point_free_head = this->point_node[this->point_free_head];
    }
    this->point_pos[point_id] = pos;
    this->point_radius[point_id] = radius;
    this->insert_to_tree(pos, radius, point_id);
    return point_id;
}

void Point_World::remove(u32 point_id) {
    assert(point_id >= 0 && point_id < this->point_node.length());
    this->erase_from_tree(point_id, this->point_pos[point_id], this->point_radius[point_id]);
    this->point_node[point_id] = this->point_free_head;
    this->point_free_head = point_id;
}

void Point_World::move_point(u32 point_id, glm::dvec2 pos, double radius) {
    assert(point_id >= 0 && point_id < this->point_node.length());
    u32 node = this->point_node[point_id];
    glm::u32vec3 node_pos = this->quadtree.get_pos_from_leaf(node);

    glm::u32vec2 wanted_node_pos = {(pos.x / 2. / this->size + 0.5) * (1 << node_pos.z), (pos.y / 2. / this->size + 0.5) * (1 << node_pos.z)};

    if (node_pos.x == wanted_node_pos.x && node_pos.y == wanted_node_pos.y && radius == this->point_radius[point_id]) {
        this->point_pos[point_id] = pos;
        return;
    }

    // Change node:
    this->erase_from_tree(point_id, this->point_pos[point_id], this->point_radius[point_id]);
    this->point_pos[point_id] = pos;
    this->point_radius[point_id] = radius;
    this->insert_to_tree(pos, radius, point_id);
}

void Point_World::find_in_radius(Array<u32>* points, glm::dvec2 pos, double radius, u32 node, glm::u32vec3 node_pos) {
    Array<u32> node_array = this->quadtree_points[node >> 2];
    for (int i = 0; i < node_array.length(); i++) {
        u32 point_id = node_array[i];
        glm::dvec2 point_pos = this->point_pos[point_id];
        double point_radius = this->point_radius[point_id];
        double disSquared = glm::length(pos - point_pos);
        disSquared *= disSquared;
        if (disSquared < radius * radius + 2 * radius * point_radius + point_radius * point_radius)
            points->push(point_id);
    }

    for (int i = 0; i < 4; i++) {
        u32 child = this->quadtree[node | i];
        if (!child) return;

        glm::u32vec3 child_pos = quadtree_calc_child_pos(node_pos, i);
        double scale = glm::pow(2, -child_pos.z);
        glm::dvec2 delta = {this->size * (-1 + 2 * child_pos.x * scale), this->size * (-1 + 2 * child_pos.y * scale)};
        delta -= pos;
        delta = {glm::abs(delta[0] - 0.5) + 0.5, glm::abs(delta[1] - 0.5) + 0.5};
        double disSquared = glm::dot(delta, delta);
        double point_radius = 2.0 * this->size * scale;
        if (0.5 * glm::sqrt(disSquared) > radius + point_radius)//if (disSquared >= radius * radius + 2 * radius * point_radius + point_radius * point_radius)
            continue;

        this->find_in_radius(points, pos, radius, child, child_pos);
    }
}

u32 Point_World::find_node_by_pos(glm::u32vec3* ref_node_pos, u32 node, glm::dvec2 pos, double radius) {
    double node_size = this->size / 2;
    glm::dvec2 local_pos = pos / 2.0 / this->size;
    //glm::dvec2 localV2_pos = {pos.x / 2.0 / this->size + 0.5, pos.y / 2.0 / this->size + 0.5};

    //var index = (((0.5 + other_pos.x / 2 / this->size - node_pos.x) * (1 << node_pos.z) < 0.5)? 0 : 1) | (((0.5 + other_pos.y / 2 / this->size - node_pos.y) * (1 << node_pos.z) < 0.5)? 0 : 2);

    while (true) {
        if (radius >= node_size) break;
        if (this->quadtree.is_leaf(node)) break;

        u32 index = ((local_pos.x < 0.5) ? 0 : 1) | ((local_pos.y < 0.5) ? 0 : 2);
        u32 child = this->quadtree[node | index];
        if (!child) assert(0); //return node;
        node = child;
        node_size /= 2;
        ref_node_pos->x *= 2;
        ref_node_pos->y *= 2;
        ref_node_pos->z++;

        if (index & 1) {
            local_pos.x -= 0.5;
            ref_node_pos->x++;
        }
        if (index & 2) {
            local_pos.y -= 0.5;
            ref_node_pos->y++;
        }
        local_pos *= 2;
    }
    return node;
}

// TODO: Split node before inserting point
void Point_World::insert_to_tree(glm::dvec2 pos, double radius, u32 point_id) {
    glm::u32vec3 node_pos = {0, 0, 0};
    u32 node = this->find_node_by_pos(&node_pos, 0, pos, radius);
    if (!this->quadtree.length()) {
        this->quadtree.resize(4);
        this->quadtree_points.resize(4);
    }
    Array<u32>* node_array_ptr = &this->quadtree_points[node >> 2];


    this->point_node[point_id] = node;
    node_array_ptr->push(point_id);

    ////////////////////////////////////////////////////////
    // Split node:
    ////////////////////////////////////////////////////////

    // Deny split
    if (node_array_ptr->length() <= POINT_WORLD_MAX_NODE_POINTS || this->quadtree.is_branch(node) || node_pos.z == 32)
        return;

    // Deny split when too large
    double node_size = this->size / 2 * glm::pow(2, -node_pos.z);
    for (int i = 0; true; i++) {
        if (i == node_array_ptr->length()) return; // Don't split
        double radius = this->point_radius[point_node[i]];
        if (radius < node_size) break; // Split
    }

    // Split:
    this->quadtree.insert_children(node);
    while (this->quadtree_points.length() < (this->quadtree.length() >> 2)) this->quadtree_points.grow(4);
    Array<u32> node_array = *node_array_ptr;
    *node_array_ptr = {0, 0, 0};

    for (int i = 0; i < node_array.length(); i++) {
        u32 other_point_id = node_array[i];
        double other_radius = this->point_radius[other_point_id];
        glm::dvec2 other_pos = this->point_pos[other_point_id];
        this->insert_to_tree(other_pos, other_radius, other_point_id);
    }
    node_array.destroy();
}

/*Point_World::_countPoints(node) {
    var node_array = this->quadtree_points[node >> 2];
    var num_points = (node_array)? node_array.length : 0;
    for (var i = 0; i < 4; i++) {
        var child = this->quadtree[node | i];
        if (child)
            num_points += this->_countPoints(child);
    }
    return num_points;
}*/

void Point_World::merge_parent(u32 child_to_merge) {
    // Deny merge of root
    if (!child_to_merge) return;

    u32 parent = this->quadtree.parents[child_to_merge >> 2];

    // Deny merge of grandparents, only parents should merge
    for (int i = 0; i < 4; i++) {
        assert(!this->quadtree.is_branch(this->quadtree[parent | i]) || this->quadtree[this->quadtree[parent | i]]);
        if (this->quadtree.is_branch(this->quadtree[parent | i])) return;
    }

    // Count the number of points, deny merge when too many
    Array<u32>* parent_array = &this->quadtree_points[parent >> 2];
    int num_points = parent_array->length();
    //if (num_points > POINT_WORLD_MAX_NODE_POINTS) return;
    for (int i = 0; i < 4; i++) {
        u32 child = this->quadtree[parent | i];
        Array<u32> child_array = this->quadtree_points[child >> 2];
        num_points += child_array.length();
        if (num_points > POINT_WORLD_MAX_NODE_POINTS) return;
    }

    // Finally merge the children
    for (int i = 0; i < 4; i++) {
        u32 child = this->quadtree[parent | i];
        Array<u32>* child_array = &this->quadtree_points[child >> 2];
        if (child_array->length() == 0) continue;
        for (int j = 0; j < child_array->length(); j++) {
            this->point_node[(*parent_array)[j]] = parent;
        }
        if (parent_array->length() == 0) {
            parent_array->destroy();
            *parent_array = *child_array;
        } else {
            parent_array->extend(*child_array);
            child_array->destroy();
        }
    }
    this->quadtree.erase_children(parent);
    /*var erasedNodes = [];
    this->quadtree.eraseChildren(parent, erasedNodes);
    for (var i = 0; i < erasedNodes.length; i++) {
        var child = erasedNodes[i];
        var child_array = this->quadtree_points[child >> 2];
        if (!child_array) continue;
        if (parent_array == null)
            this->quadtree_points[parent >> 2] = child_array;
        else
            this->quadtree_points[parent >> 2] = parent_array.concat(child_array);
        parent_array = this->quadtree_points[parent >> 2];
        this->quadtree_points[child >> 2] = null;
    }*/

    // Merge grandParent
    this->merge_parent(parent);
}

void Point_World::erase_from_tree(u32 point_id, glm::dvec2 pos, double radius) {
    u32 node = this->point_node[point_id];
    glm::u32vec3 node_pos = this->quadtree.get_pos_from_leaf(node);
    glm::u32vec3 node_pos2 = {0, 0, 0};
    assert(node == this->find_node_by_pos(&node_pos2, 0, pos, radius));
    assert(node_pos.x == node_pos2.x && node_pos.y == node_pos2.y && node_pos.z == node_pos2.z);
    Array<u32>* node_array = &this->quadtree_points[node >> 2];
    assert(node_array->length());
    if (node_array->length() == 1) {
        this->quadtree_points[node >> 2].clear();
    } else {
        for (int i = 0; true; i++) {
            assert (i != node_array->length());
            if ((*node_array)[i] != point_id) continue;
            node_array->splice(i, 1);
            break;
        }
    }
    this->merge_parent(node);
}
