#include "core/common.h"
#include "entity_manager.h"
#include "component/name.h"
#include "component/pos.h"
#include "component/movement.h"
#include "entity/player.h"
#include "core/quadtree.h"

#include <SDL2/SDL.h>
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
u32 entity_player;

int main(int argc, char* argv[]) {
    printf("!!!\n");

    init();

    // Game loop:
#ifdef __EMSCRIPTEN__
    EM_ASM(console.log("console.log from cpp-file using EM_ASM!"));
    emscripten_set_main_loop(update, 60, 1);
#else
    uint32_t frame_time, last_frame_time;
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
    // Bad test
    quadtree.init();
    u32 node = quadtree.insert_node({1, 0, 0});
    quadtree.insert_children(node);
    quadtree.erase_node(node);
    quadtree.destroy();

    entity_manager_init<Component_Name, Component_Pos, Component_Movement, Component_Sprite>();
    entity_player = entity_create();
    entity_player_create(entity_player);
    for (int i = 0; i < 100000; i++) {
        u32 entity = entity_create();
        entity_player_create(entity, {800 + glm::cos((double)i*59)*700.0, -450 + (double)-glm::sin(((double)i)*117.)*400}, { i%256, (i/256)%256, (i/256/256)%256, 1 });
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

    // The window is open: could enter program loop here (see SDL_PollEvent())

    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
}

void deinit() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void update() {
    SDL_Event event;
    while(!quit && SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) quit = true;
            switch(event.key.keysym.sym) {
            case SDLK_w:
                component_get<Component_Movement>(entity_player)->dir |= MOVEMENT_UP;
                break;
            case SDLK_a:
                component_get<Component_Movement>(entity_player)->dir |= MOVEMENT_LEFT;
                break;
            case SDLK_s:
                component_get<Component_Movement>(entity_player)->dir |= MOVEMENT_DOWN;
                break;
            case SDLK_d:
                component_get<Component_Movement>(entity_player)->dir |= MOVEMENT_RIGHT;
                break;
            }

            break;
        case SDL_KEYUP:
            switch(event.key.keysym.sym) {
            case SDLK_w:
                component_get<Component_Movement>(entity_player)->dir &= ~MOVEMENT_UP;
                break;
            case SDLK_a:
                component_get<Component_Movement>(entity_player)->dir &= ~MOVEMENT_LEFT;
                break;
            case SDLK_s:
                component_get<Component_Movement>(entity_player)->dir &= ~MOVEMENT_DOWN;
                break;
            case SDLK_d:
                component_get<Component_Movement>(entity_player)->dir &= ~MOVEMENT_RIGHT;
                break;
            }
            break;
        default:
            break;
        }
    }

    double dt = 1./60.;

    component_movement_update();

    render(dt);
}

int g_x = 0;

void render(double dt) {
    g_x++;
    // CLear
    SDL_SetRenderDrawColor( renderer, 0x44, 0x44, 0x44, 0xFF );
    SDL_RenderClear(renderer);

    component_sprite_render(renderer);

    // Finish
    SDL_RenderPresent( renderer );
}
