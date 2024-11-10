#include "newton_defs.h"

int main(void) {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Orbits",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          800, 600,
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

    solar_system sol;
    motion m;
    init_solar_system(&sol, &m);

    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            // if (e.type == SDL_KEYDOWN) {
            //
            //     update_motion(&e, &m, &quit);
            // }
        }
        update_render(renderer, &sol, &m);
        SDL_RenderPresent(renderer);
        usleep(SLEEP_TIME);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}

void draw_sun(SDL_Renderer* renderer, body* b) {

    for (int y = -(b->radius); y <= b->radius; y++) {
        long double dx = sqrt(b->radius * b->radius - y * y); // Calculate x distance based on y
        long double startX = b->pos.x - dx;
        long double endX = b->pos.x + dx;
        SDL_RenderDrawLine(renderer, startX, b->pos.y + y, endX, b->pos.y + y); // Draw horizontal line
    }
}

void draw_circle(SDL_Renderer* renderer, body* b) {

    for (int y = -(b->radius); y <= b->radius; y++) {
        long int dx = (int) sqrt(b->radius * b->radius - y * y); // Calculate x distance based on y
        long int startX = b->pos.x + 400 - dx;
        long int endX = b->pos.x + 400 + dx;

        long int y1 = b->pos.y + 300 + y;
        printf("%ld\n", y1);
        long int y2 = b->pos.y + 300 + y;
        SDL_RenderDrawLine(renderer, startX, y1 , endX, y2); // Draw horizontal line
    }
}

void update_earth_orbit(solar_system* sol) {

    // if ((int) earth->angle >= COMPLETED_ORBIT) {
    //     earth->angle = 0;
    // }
    // earth->angle += m->direction * m->scale * EARTH_INC;

    long double r = sqrt(pow(sol->earth.pos.x, 2) + pow(sol->earth.pos.y, 2));
    long double ax = -G * sol->sun.mass / sol->earth.pos.x * pow(r, 1);
    long double ay = -G * sol->sun.mass / sol->earth.pos.y * pow(r, 1);

    sol->earth.vel.x += ax * DT;
    sol->earth.vel.y += ay * DT;

    sol->earth.pos.x += sol->earth.vel.x * DT;
    sol->earth.pos.y += sol->earth.vel.y * DT;

}

// void update_moon_orbit(body* earth, body* moon, motion* m) {
//
//     if ((int) moon->angle >= COMPLETED_ORBIT) {
//         moon->angle = 0;
//     }
//     moon->angle += m->direction * m->scale * MOON_INC;
//
//     double angle = moon->angle * ANGLE_INC;
//     moon->x = earth->x + (int) (moon->orb_rad * cos(angle));
//     moon->y = earth->y + (int) (moon->orb_rad * sin(angle));
// }

void init_solar_system(solar_system* sol, motion* m) {

    sol->sun.pos.x = SOLAR_X; sol->sun.pos.y = SOLAR_Y;
    sol->sun.vel.x = 0; sol->sun.vel.y = 0;
    sol->sun.radius = SOLAR_RADIUS;
    sol->earth.radius = EARTH_RADIUS;
    sol->earth.pos.x = 250; sol->earth.pos.y = 0;
    sol->earth.vel.x = 0; sol->earth.vel.y = 5;
    sol->sun.mass = 1.989*pow(10, 1);
    sol->earth.mass = 5.972; //*pow(10, 1);
    //sol->moon.radius = MOON_RADIUS;
    //sol->earth.angle = sol->moon.angle = 0;

    m->rate = medium;
    m->scale = pow(1.5, m->rate);
    m->current = m->rate;
    m->direction = forwards;
}

// void update_motion(SDL_Event* e, motion* m, bool* quit) {
//
//     switch (e->key.keysym.sym) {
//
//         case SDLK_ESCAPE:
//             *quit = true;
//             break;
//
//         case SDLK_UP:
//
//             if (m->rate < fast) {
//                 m->current = m->scale = pow(1.5, ++m->rate);
//             }
//             break;
//
//         case SDLK_DOWN:
//
//             if (m->rate > slow) {
//                 m->current = m->scale = pow(1.5, --m->rate);
//             }
//             else {
//                 m->scale = 0;
//             }
//             break;
//
//         case SDLK_s:
//
//             if (m->scale == 0) {
//                 m->rate = m->current;
//                 m->scale = pow(1.5, m->rate);
//             }
//             else {
//                 m->current = m->rate;
//                 m->scale = 0;
//                 m->rate = slow;
//             }
//             break;
//
//         case SDLK_r:
//             m->direction = -m->direction;
//             break;
//
//         default:
//             break;
//     }
// }

void update_render(SDL_Renderer* renderer, solar_system* sol, motion* m) {

    // Clear background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw Sun
    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
    draw_sun(renderer, &sol->sun);

    // Draw Earth
    update_earth_orbit(sol);
    SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);
    draw_circle(renderer, &sol->earth);

    // // Draw Moon
    // update_moon_orbit(&sol->earth, &sol->moon, m);
    // SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    // draw_circle(renderer, &sol->moon);
}
