#pragma once

#include "core/common.h"

u32 g_msg_tick = 0;

@msg tick() {
    printf("Tick!");
    g_msg_tick++;
}

@msg cmd_move(u32 ent, u8 input, f32 rel_mouse_x, f32 rel_mouse_y) {
    //set_pos(cmd.ent, cmd.ent_move.pos);
    //set_vel(cmd.ent, cmd.ent_move.vel);
    auto move = ent_get(ent, Move);
    move->dir = 
        input;
    //move->update_input(cmd.ent_move.input);
    //move->update_mouse_pos(cmd.ent_move.rel_mouse);
}

@msg msg_move(u8 input, f32 rel_mouse_x, f32 rel_mouse_y) {
    cmd_move(@plr->ent, input, rel_mouse_x, rel_mouse_y);
}