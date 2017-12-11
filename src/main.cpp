#include "core/common.h"
#include "ent_manager.h"
#include "ent/name.h"
#include "ent/physics.h"
#include "ent/movement.h"
#include "entities.h"
#include "core/quadtree.h"
#include "point_world.h"
#include "cmd.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

void init();
void deinit();
void update(); // TODO: calc dt and send to render
void render(double dt); // called by update()

SDL_Window *window;
SDL_Renderer* renderer;
bool quit = false;
u32 ent_player;

int main(int argc, char** argv) {
    printf("!!!\n");
    Point_World point_world;
    point_world.init(10.0);
    point_world.add({0.5, 0.7}, 0.1);
    point_world.deinit();
    init();

    // Game loop:
#ifdef __EMSCRIPTEN__
    EM_ASM(console.log("console.log from cpp-file using EM_ASM!"));
    emscripten_set_main_loop(update, 60, 1);
#else
    uint32_t frame_time=SDL_GetTicks(), last_frame_time;
    while (!quit) {
        update();

        last_frame_time = frame_time;
        frame_time = SDL_GetTicks();
        uint32_t delta_time = frame_time - last_frame_time;
        int32_t sleep_time = 1000/60 - delta_time;
        if (sleep_time > 0) SDL_Delay(sleep_time);
    }
    deinit();
#endif
    return 0;
}

void init() {
    Quadtree quadtree;
    quadtree.init();
    // Bad test
    Point_World point_world;
    point_world.init(10.0);
    point_world.size = 10.0;
    SCOPE_EXIT(point_world.deinit());
    point_world.add({0.3, 0.6}, 0.9);
    u32 node = quadtree.insert_node(0 | 1);
    quadtree.insert_children(node);
    quadtree.insert_children(quadtree[node | 3]);
    quadtree.erase_children(quadtree[node | 3]);
    quadtree.erase_node(0 | 1);
    quadtree.destroy();

    ent_manager_init<Ent_Name, Ent_Physics, Ent_Movement, Ent_Sprite>();
    ent_player = ent_create();
    entity_player_cerate(ent_player);
    for (int i = 0; i < 1000; i++) {
        u32 ent = ent_create();
        entity_player_cerate(ent, {800 + glm::cos((double)i*59)*700.0, -450 + (double)-glm::sin(((double)i)*117.)*400}, { i%256, (i/256)%256, (i/256/256)%256, 1 });
    }
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(
        "An SDL2 window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        1600,
        900,
        SDL_WINDOW_OPENGL
    );

    if (!window) {
        printf("Could not create window: %s\n", SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
}

void deinit() {
	ent_manager_deinit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void update() {
	auto movement = ent_get(ent_player, Movement);

    static u16 move_dir = 0;

    SDL_Event event;
    while (!quit && SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) quit = true;
            switch (event.key.keysym.sym) {
            case SDLK_w:
                move_dir |= MOVEMENT_UP;
                break;
            case SDLK_a:
                move_dir |= MOVEMENT_LEFT;
                break;
            case SDLK_s:
                move_dir |= MOVEMENT_DOWN;
                break;
            case SDLK_d:
                move_dir |= MOVEMENT_RIGHT;
                break;
            }

            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
            case SDLK_w:
                move_dir &= ~MOVEMENT_UP;
                break;
            case SDLK_a:
                move_dir &= ~MOVEMENT_LEFT;
                break;
            case SDLK_s:
                move_dir &= ~MOVEMENT_DOWN;
                break;
            case SDLK_d:
                move_dir &= ~MOVEMENT_RIGHT;
                break;
            }
            break;
        default:
            break;
        }
    }
    if (movement && movement->dir != move_dir) {
        Cmd cmd;
        cmd.type = CMD_ENT_MOVE;
        cmd.ent = ent_player;
        cmd.ent_move.input = move_dir;
        cmd.ent_move.rel_mouse = { 0.f, 0.f };
        push(cmd);
    }
    cmd_begin_tick();
    cmd_end_tick();

    double dt = 1./60.;

    ent_movement_update();

    render(dt);
}

int g_x = 0;

void render(double dt) {
    g_x++;
    // CLear
    SDL_SetRenderDrawColor( renderer, 0x44, 0x44, 0x44, 0xFF );
    SDL_RenderClear(renderer);

    ent_sprite_render(renderer);

    // Finish
    SDL_RenderPresent( renderer );
}