#include "newton2_defs.h"

int main(void) {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Orbits",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          960, 600,
                                          SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Event e;
    bool isFullscreen = false;

    bool quit = false;
    solar_system sol;

    double x = EARTH_POS_X;
    double y = EARTH_POS_Y;
    double vx = EARTH_VEL_X;
    double vy = EARTH_VEL_Y;

    int X = 650;
    int Y = 300;




    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;

                    case SDLK_f:  // Toggle fullscreen with 'F' key
                        isFullscreen = !isFullscreen;
                    SDL_SetWindowFullscreen(window, isFullscreen ? SDL_WINDOW_FULLSCREEN : 0);
                    break;

                    default:
                        break;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
        draw_sun(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);
        update_orbit(&x, &y, &vx, &vy);
        screen(&X, &Y, &x, &y);
        draw_earth(renderer, X, Y);


        SDL_RenderPresent(renderer);
        usleep(SLEEP_TIME);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}

void init_solar_system(solar_system* sol) {

    sol->earth.pos.x = EARTH_POS_X;
    sol->earth.pos.y = EARTH_POS_Y;
    sol->earth.vel.x = EARTH_VEL_X;
    sol->earth.vel.y = EARTH_VEL_Y;
    sol->earth.pos.screen_x = EARTH_POS_X / SF + SOLAR_X;
    sol->earth.pos.screen_y = EARTH_POS_Y / SF + SOLAR_Y;

    // Independent of gravitational calculations
    sol->earth.radius = 10;
    sol->earth.radius = 25;
}

// void update_orbit(body* b) {
//
//     double r = sqrt(b->pos.x * b->pos.x + b->pos.y * b->pos.y);
//
//     // Calculate gravitational acceleration components
//     double ax = -G * S_MASS / (r * r * r) * b->pos.x;
//     double ay = -G * S_MASS / (r * r * r) * b->pos.y;
//
//     // Update velocity components
//     b->vel.x += ax * DT;
//     b->vel.y += ay * DT;
//
//     // Update position components
//     b->pos.x += b->vel.x * DT;
//     b->pos.y += b->vel.y * DT;
//
//     // Update position on screen
//     b->pos.screen_x = b->pos.x / SF + SOLAR_X;
//     b->pos.screen_y = b->pos.y / SF + SOLAR_Y;
// }

void update_orbit(double* x, double* y, double* vx, double* vy) {

    double r = sqrt(*x * *x + *y * *y);

    // Calculate gravitational acceleration components
    double ax = -G * S_MASS / (r * r * r) * *x;
    double ay = -G * S_MASS / (r * r * r) * *y;

    // Update velocity components
    *vx += ax * DT;
    *vy += ay * DT;

    // Update position components
    *x += *vx * DT;
    *y += *vy * DT;
}

void draw_sun(SDL_Renderer* renderer) {

    for (int y = -25; y <= 25; y++) {
        long double dx = sqrt(25 * 25 - y * y); // Calculate x distance based on y
        long double startX = SOLAR_X - dx;
        long double endX = SOLAR_X + dx;
        SDL_RenderDrawLine(renderer, startX, SOLAR_Y + y, endX, SOLAR_Y + y); // Draw horizontal line
    }
}

void draw_earth(SDL_Renderer* renderer, int X, int Y) {

    for (int y = -10; y <= 10; y++) {
        long double dx = sqrt(10 * 10 - y * y); // Calculate x distance based on y
        long double startX = X - dx;
        long double endX = X + dx;
        SDL_RenderDrawLine(renderer, startX, Y + y, endX, Y + y); // Draw horizontal line
    }
}

void screen(int* X, int* Y, double* x, double* y) {

    printf("%f\n", *x);

    *X = *x / SF + SOLAR_X;
    *Y = *y / SF + SOLAR_Y;
}
