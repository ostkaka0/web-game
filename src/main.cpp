#include "core/common.h"
#include "entity_manager.h"
#include "component/name.h"

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
    ENTITY_MANAGER_INIT(Entity_Name, Entity_Name2, Entity_Name3);
    entity_add(0, Entity_Name{ "HALLÅ DÄR" });
    entity_add(0, Entity_Name2{ "HALLÅ DÄR" });
    entity_remove<Entity_Name>(0);
    entity_remove<Entity_Name2>(0);
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(
        "An SDL2 window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
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
            if (event.key.keysym.sym == SDLK_ESCAPE)
            quit = true;
            break;
        default:
            break;
        }
    }

    double dt = 1./60.;
    render(dt);
}

int g_x = 0;

void render(double dt) {
    g_x++;
    // CLear
    SDL_SetRenderDrawColor( renderer, 0x44, 0x44, 0x44, 0xFF );
    SDL_RenderClear(renderer);

    // Draw rect
    SDL_Rect fillRect = { g_x, 40, 50, 50 };
    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0x00, 0xFF );
    SDL_RenderFillRect( renderer, &fillRect );

    // Finish
    SDL_RenderPresent( renderer );
}
