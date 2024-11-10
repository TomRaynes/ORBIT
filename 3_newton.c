#include "newton3_defs.h"

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
    init_solar_system(&sol);
    int speed = medium;
    double rate = pow(1.5, speed);
    int gravity = medium;
    double g = pow(1.5, gravity);

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

                    case SDLK_f:
                        isFullscreen = !isFullscreen;
                    SDL_SetWindowFullscreen(window, isFullscreen ? SDL_WINDOW_FULLSCREEN : 0);
                    break;

                    case SDLK_RIGHT:

                        if (speed < fast) {
                            speed++;
                            rate = pow(1.5, speed);
                        }
                    break;

                    case SDLK_LEFT:

                        if (speed > min_speed) {
                            speed--;
                            rate = pow(1.5, speed);
                        }
                        else {
                            rate = 0;
                        }
                    break;

                    case SDLK_UP:

                        if (gravity < fast) {
                            gravity++;
                            g = pow(1.5, gravity);
                        }
                    break;

                    case SDLK_DOWN:

                        if (gravity > slow) {
                            gravity--;
                            g = pow(1.5, gravity);
                        }
                    break;

                    case SDLK_SPACE:

                        init_solar_system(&sol);
                        speed = medium;
                        rate = pow(1.5, speed);
                        gravity = medium;
                        g = pow(1.5, gravity);
                        break;

                    default:
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        // SDL_RenderDrawLine(renderer, 1, 50, 960, 50);

        SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
        draw_body(renderer, &sol.sun);

        check_body_position(&sol.earth);

        if (!sol.earth.destroyed) {

            SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);
            update_orbit(&sol.earth, rate, g);
            draw_body(renderer, &sol.earth);

            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
            //update_moon_gravitational_centre(&sol);
            update_moon_orbit(&sol.earth, &sol.moon, rate, g);
            //moon(&sol.moon);
            draw_body(renderer, &sol.moon);
        }

        SDL_RenderPresent(renderer);
        //usleep(SLEEP_TIME);
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
    sol->earth.destroyed = false;
    sol->earth.pos.Gx = SOLAR_X;
    sol->earth.pos.Gy = SOLAR_Y;
    sol->earth.Gmass = S_MASS;
    sol->earth.pos.screen_x = EARTH_POS_X / SF + SOLAR_X;
    sol->earth.pos.screen_y = EARTH_POS_Y / SF + SOLAR_Y;

    sol->moon.pos.x = MOON_POS_X;
    sol->moon.pos.y = MOON_POS_Y;
    sol->moon.vel.x = MOON_VEL_X;
    sol->moon.vel.y = MOON_VEL_Y;
    sol->moon.Gmass = E_MASS;
    sol->moon.pos.Gx = sol->earth.pos.x / SF + SOLAR_X;
    sol->moon.pos.Gy = sol->earth.pos.y / SF + SOLAR_Y;

    // Independent of gravitational calculations
    sol->earth.radius = EARTH_RADIUS;
    sol->moon.radius = MOON_RADIUS;
    sol->sun.radius = SOLAR_RADIUS;
    sol->sun.pos.screen_x = SOLAR_X;
    sol->sun.pos.screen_y = SOLAR_Y;

}

void update_orbit(body* b, double rt, double g) {

    double r = sqrt(b->pos.x * b->pos.x + b->pos.y * b->pos.y);

    double ax = -G * S_MASS / (r * r * r) * b->pos.x;
    double ay = -G * S_MASS / (r * r * r) * b->pos.y;

    b->vel.x += ax * DT * rt * g;
    b->vel.y += ay * DT * rt * g;

    b->pos.x += b->vel.x * DT * rt;
    b->pos.y += b->vel.y * DT * rt;

    // Update position on screen
    b->pos.screen_x = b->pos.x / SF + SOLAR_X;
    b->pos.screen_y = b->pos.y / SF + SOLAR_Y;

}

void update_moon_orbit(body* earth, body* b, double rt, double g) {

    double r = sqrt(b->pos.x * b->pos.x + b->pos.y * b->pos.y);

    double ax = -G * E_MASS / (r * r * r) * b->pos.x;
    double ay = -G * E_MASS / (r * r * r) * b->pos.y;

    b->vel.x += ax * DT * rt * g;
    b->vel.y += ay * DT * rt * g;

    b->pos.x += b->vel.x * DT * rt;
    b->pos.y += b->vel.y * DT * rt;

    // Update position on screen
    b->pos.screen_x = b->pos.x / SF + earth->pos.screen_x;
    b->pos.screen_y = b->pos.y / SF + earth->pos.screen_y;

}

void draw_body(SDL_Renderer* ren, body* b) {

    for (int y = -b->radius; y <= b->radius; y++) {
        long double dx = sqrt(b->radius * b->radius - y * y);
        long double startX = b->pos.screen_x - dx;
        long double endX = b->pos.screen_x + dx;
        int screen_y = b->pos.screen_y;
        SDL_RenderDrawLine(ren, startX, screen_y + y, endX, screen_y + y);
    }
}

void check_body_position(body* b) {

    if (sqrt(pow(b->pos.x, 2) + pow(b->pos.y, 2)) / SF < SOLAR_RADIUS) {
        b->destroyed = true;
    }
}

void update_moon_gravitational_centre(solar_system* sol) {

    sol->moon.pos.Gx = sol->earth.pos.screen_x;
    sol->moon.pos.Gy = sol->earth.pos.screen_y;
}

void moon(body* b) {

    b->pos.screen_x += 50;
    b->pos.screen_y += 50;
}
