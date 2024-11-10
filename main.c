#include "defs.h"

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
            if (e.type == SDL_KEYDOWN) {

                update_motion(&e, &m, &quit);
            }
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

void draw_circle(SDL_Renderer* renderer, body* b) {

    for (int y = -(b->radius); y <= b->radius; y++) {
        int dx = (int) sqrt(b->radius * b->radius - y * y); // Calculate x distance based on y
        int startX = b->x - dx;
        int endX = b->x + dx;
        SDL_RenderDrawLine(renderer, startX, b->y + y, endX, b->y + y); // Draw horizontal line
    }
}

void update_earth_orbit(body* earth, motion* m) {

    if ((int) earth->angle >= COMPLETED_ORBIT) {
        earth->angle = 0;
    }
    earth->angle += m->direction * m->scale * EARTH_INC;

    double angle = earth->angle * ANGLE_INC;
    earth->x = SOLAR_X + (int) (earth->orb_rad * cos(angle));
    earth->y = SOLAR_Y + (int) (earth->orb_rad * sin(angle));
}

void update_moon_orbit(body* earth, body* moon, motion* m) {

    if ((int) moon->angle >= COMPLETED_ORBIT) {
        moon->angle = 0;
    }
    moon->angle += m->direction * m->scale * MOON_INC;

    double angle = moon->angle * ANGLE_INC;
    moon->x = earth->x + (int) (moon->orb_rad * cos(angle));
    moon->y = earth->y + (int) (moon->orb_rad * sin(angle));
}

void init_solar_system(solar_system* sol, motion* m) {

    sol->sun.x = SOLAR_X; sol->sun.y = SOLAR_Y;
    sol->sun.radius = SOLAR_RADIUS;
    sol->earth.radius = EARTH_RADIUS;
    sol->earth.orb_rad = EARTH_ORBITAL_RADIUS;
    sol->moon.radius = MOON_RADIUS;
    sol->moon.orb_rad = MOON_ORBITAL_RADIUS;
    sol->earth.angle = sol->moon.angle = 0;

    m->rate = medium;
    m->scale = pow(1.5, m->rate);
    m->current = m->rate;
    m->direction = forwards;
}

void update_motion(SDL_Event* e, motion* m, bool* quit) {

    switch (e->key.keysym.sym) {

        case SDLK_ESCAPE:
            *quit = true;
            break;

        case SDLK_UP:

            if (m->rate < fast) {
                m->current = m->scale = pow(1.5, ++m->rate);
            }
            break;

        case SDLK_DOWN:

            if (m->rate > slow) {
                m->current = m->scale = pow(1.5, --m->rate);
            }
            else {
                m->scale = 0;
            }
            break;

        case SDLK_s:

            if (m->scale == 0) {
                m->rate = m->current;
                m->scale = pow(1.5, m->rate);
            }
            else {
                m->current = m->rate;
                m->scale = 0;
                m->rate = slow;
            }
            break;

        case SDLK_r:
            m->direction = -m->direction;
            break;

        default:
            break;
    }
}

void update_render(SDL_Renderer* renderer, solar_system* sol, motion* m) {

    // Clear background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw Sun
    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
    draw_circle(renderer, &sol->sun);

    // Draw Earth
    update_earth_orbit(&sol->earth, m);
    SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);
    draw_circle(renderer, &sol->earth);

    // Draw Moon
    update_moon_orbit(&sol->earth, &sol->moon, m);
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    draw_circle(renderer, &sol->moon);
}
