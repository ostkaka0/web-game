#pragma once

#include "core\common.h"
#include "core\array.h"
#include "globals.h"

#include <winsock2.h>
#include <glm/glm.hpp>

enum Enum_Cmd {
    CMD_NULL = 0,
    CMD_ENT_MOVE,

    CMD_ENT_BEGIN = CMD_ENT_MOVE
};

struct Cmd {
    u16 type;
    union {
        u32 ent;
        struct {
            u32 ent;
            u8 input;
            glm::vec2 rel_mouse; // Cursor position relative to world
        } ent_move;
    };
};

Array<Cmd> g_cmds = {};

void push(Cmd cmd) {
    if (!g_is_server) return;
    g_cmds.push(cmd);
}

void init(Cmd* cmd) {
    switch (cmd->type) {
    }
}

void run(Cmd cmd) {
    switch (cmd.type) {
    case CMD_ENT_MOVE: {
        //set_pos(cmd.ent, cmd.ent_move.pos);
        //set_vel(cmd.ent, cmd.ent_move.vel);
        auto move = ent_get(cmd.ent, Move);
        move->dir = cmd.ent_move.input;
        //move->update_input(cmd.ent_move.input);
        //move->update_mouse_pos(cmd.ent_move.rel_mouse);
        break;
    }
    default:
        assert(0);
        break;
    }
}

void endian_swap(Cmd* cmd) {
    if (cmd->type >= CMD_ENT_BEGIN) {
        cmd->ent = htonl(cmd->ent);
    }
    switch (cmd->type) {
    case CMD_ENT_MOVE:
        cmd->ent_move.rel_mouse.x = htonf(cmd->ent_move.rel_mouse.x);
        cmd->ent_move.rel_mouse.y = htonf(cmd->ent_move.rel_mouse.y);
        break;
    default:
        assert(0);
        break;
    }
}

void cmd_begin_tick() {
    for (int i = 0; i < g_cmds.length(); i++) {
        init(&g_cmds[i]);
    }
}

void cmd_end_tick() {
    for (int i = 0; i < g_cmds.length(); i++) {
        run(g_cmds[i]);
    }
    g_cmds.clear();
}