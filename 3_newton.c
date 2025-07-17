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

                    case SDLK_d:
                        sol.earth.destroyed = true;
                        break;

                    case SDLK_m:

                        if (sol.zoom == true) {
                            sol.zoom = false;
                            sol.sun.radius /= 1.5;

                            sol.earth.pos.x /= 2;
                            sol.earth.pos.y /= 2;
                            sol.earth.vel.x *= 1.4;
                            sol.earth.vel.y *= 1.4;
                            sol.earth.radius /= 1.5;

                            sol.moon.pos.x /= 2;
                            sol.moon.pos.y /= 2;
                            sol.moon.vel.x *= 1.4;
                            sol.moon.vel.y *= 1.4;
                            sol.moon.radius /= 1.5;
                        }
                        else {
                            sol.zoom = true;
                            sol.sun.radius *= 1.5;

                            sol.earth.pos.x *= 2;
                            sol.earth.pos.y *= 2;
                            sol.earth.vel.x /= 1.4;
                            sol.earth.vel.y /= 1.4;
                            sol.earth.radius *= 1.5;

                            sol.moon.pos.x *= 2;
                            sol.moon.pos.y *= 2;
                            sol.moon.vel.x /= 1.4;
                            sol.moon.vel.y /= 1.4;
                            sol.moon.radius *= 1.5;
                        }
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

        SDL_SetRenderDrawColor(renderer, 252, 231, 35, 255);
        draw_body(renderer, &sol.sun);

        check_earth_position(&sol, &sol.earth);
        check_body_position(&sol.moon, &sol.sun);
        check_body_position(&sol.mars, &sol.sun);
        check_body_position(&sol.jupiter, &sol.sun);
        check_body_position(&sol.saturn, &sol.sun);

        if (!sol.earth.destroyed) {
            SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);
            update_orbit(sol.bodies[1], &sol, rate, g);
            draw_body(renderer, &sol.earth);
        }

        if (!sol.moon.destroyed) {
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
            update_moon_gravitational_centre(&sol);
            update_orbit(sol.bodies[2], &sol, rate, g);
            draw_body(renderer, &sol.moon);
        }

        if (sol.zoom == false && !sol.mars.destroyed) {
            SDL_SetRenderDrawColor(renderer, 245, 125, 56, 255);
            update_orbit(sol.bodies[3], &sol, rate, g);
            draw_body(renderer, &sol.mars);
        }

        if (sol.zoom == false && !sol.jupiter.destroyed) {
            SDL_SetRenderDrawColor(renderer, 250, 204, 145, 255);
            update_orbit(sol.bodies[4], &sol, rate, g);
            draw_jupiter(renderer, &sol.jupiter);
        }

        if (sol.zoom == false && !sol.saturn.destroyed) {
            SDL_SetRenderDrawColor(renderer, 200, 200, 150, 255);
            update_orbit(sol.bodies[5], &sol, rate, g);
            draw_saturn(renderer, &sol.saturn);
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

    sol->zoom = true;

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
    sol->moon.destroyed = false;
    sol->moon.pos.Gx = sol->earth.pos.x / SF + SOLAR_X;
    sol->moon.pos.Gy = sol->earth.pos.y / SF + SOLAR_Y;
    sol->moon.pos.screen_x = sol->moon.pos.x / SF + sol->moon.pos.Gx;
    sol->moon.pos.screen_y = sol->moon.pos.y / SF + sol->moon.pos.Gy;

    sol->mars = sol->earth;
    sol->mars.pos.x /= 1.4;
    sol->mars.pos.y /= 1.4;
    sol->mars.vel.x *= 1.15;
    sol->mars.vel.y *= 1.15;
    sol->mars.radius = 6;

    sol->jupiter = sol->earth;
    sol->jupiter.radius = 12;

    sol->saturn = sol->earth;
    sol->saturn.pos.x *= 1.4;
    sol->saturn.pos.y *= 1.4;
    sol->saturn.vel.x /= 1.15;
    sol->saturn.vel.y /= 1.15;
    sol->saturn.radius = 11;

    // populate bodies array
    sol->bodies[0] = &sol->sun;
    sol->bodies[1] = &sol->earth;
    sol->bodies[2] = &sol->moon;
    sol->bodies[3] = &sol->mars;
    sol->bodies[4] = &sol->jupiter;
    sol->bodies[5] = &sol->saturn;


    // Independent of gravitational calculations
    sol->earth.radius = EARTH_RADIUS;
    sol->moon.radius = MOON_RADIUS;
    sol->sun.radius = SOLAR_RADIUS;
    sol->sun.pos.screen_x = SOLAR_X;
    sol->sun.pos.screen_y = SOLAR_Y;

}

// void update_orbit(body* b, double rt, double g) {
//
//     double r = sqrt(b->pos.x * b->pos.x + b->pos.y * b->pos.y);
//
//     double ax = -G * b->Gmass / (r * r * r) * b->pos.x;
//     double ay = -G * b->Gmass / (r * r * r) * b->pos.y;
//
//     b->vel.x += ax * DT * rt * g;
//     b->vel.y += ay * DT * rt * g;
//
//     b->pos.x += b->vel.x * DT * rt;
//     b->pos.y += b->vel.y * DT * rt;
//
//     // Update position on screen
//     b->pos.screen_x = b->pos.x / SF + b->pos.Gx;
//     b->pos.screen_y = b->pos.y / SF + b->pos.Gy;
// }

void update_orbit(body* b, solar_system* sol, double rt, double g) {
    if (b->destroyed) return;

    double ax = 0.0, ay = 0.0;

    for (int i = 0; i < BODY_COUNT; i++) {
        body* other = sol->bodies[i];
        if (other == b || other->destroyed) continue;

        double dx = other->pos.x - b->pos.x;
        double dy = other->pos.y - b->pos.y;
        double dist_sq = dx * dx + dy * dy;

        // Avoid division by zero
        if (dist_sq < 1e5) continue;

        double dist = sqrt(dist_sq);
        double force = G * other->Gmass / (dist_sq * dist);  // Gm/r^3

        ax += force * dx;
        ay += force * dy;
    }

    b->vel.x += ax * DT * rt * g;
    b->vel.y += ay * DT * rt * g;

    b->pos.x += b->vel.x * DT * rt;
    b->pos.y += b->vel.y * DT * rt;

    b->pos.screen_x = b->pos.x / SF + b->pos.Gx;
    b->pos.screen_y = b->pos.y / SF + b->pos.Gy;
}

// void update_moon_orbit(body* earth, body* b, double rt, double g) {
//
//     double r = sqrt(b->pos.x * b->pos.x + b->pos.y * b->pos.y);
//
//     double ax = -G * E_MASS / (r * r * r) * b->pos.x;
//     double ay = -G * E_MASS / (r * r * r) * b->pos.y;
//
//     b->vel.x += ax * DT * rt * g;
//     b->vel.y += ay * DT * rt * g;
//
//     b->pos.x += b->vel.x * DT * rt;
//     b->pos.y += b->vel.y * DT * rt;
//
//     // Update position on screen
//     b->pos.screen_x = b->pos.x / SF + earth->pos.screen_x;
//     b->pos.screen_y = b->pos.y / SF + earth->pos.screen_y;
//
// }

void draw_body(SDL_Renderer* ren, body* b) {

    for (int y = -b->radius; y <= b->radius; y++) {
        long double dx = sqrt(b->radius * b->radius - y * y);
        long double startX = b->pos.screen_x - dx;
        long double endX = b->pos.screen_x + dx;
        int screen_y = b->pos.screen_y;
        SDL_RenderDrawLine(ren, startX, screen_y + y, endX, screen_y + y);
    }
}

void draw_jupiter(SDL_Renderer* ren, body* b) {

    draw_body(ren, b);

    int width = 11, height = 9;

    int halfWidth = width / 2;
    int halfHeight = height / 2;

    for (int y = -halfHeight; y <= halfHeight; y++) {
        int dx = (int)(halfWidth * sqrt(1 - (y * y) / (float) (halfHeight * halfHeight)));

        int startX = b->pos.screen_x + 3 - dx;
        int endX = b->pos.screen_x + 3 + dx;
        int centerY = b->pos.screen_y + 3;
        SDL_SetRenderDrawColor(ren, 212, 91, 21, 255);
        SDL_RenderDrawLine(ren, startX, centerY + y, endX, centerY + y);
    }

    width = 9, height = 7;

    halfWidth = width / 2;
    halfHeight = height / 2;

    for (int y = -halfHeight; y <= halfHeight; y++) {
        int dx = (int)(halfWidth * sqrt(1 - (y * y) / (float) (halfHeight * halfHeight)));

        int startX = b->pos.screen_x + 3 - dx;
        int endX = b->pos.screen_x + 3 + dx;
        int centerY = b->pos.screen_y + 3;
        SDL_SetRenderDrawColor(ren, 250, 144, 82, 255);
        SDL_RenderDrawLine(ren, startX, centerY + y, endX, centerY + y);
    }
}

void draw_saturn(SDL_Renderer* ren, body* b) {

    draw_body(ren, b);

    int x1 = b->pos.screen_x - 2.2* b->radius;
    int x2 = b->pos.screen_x + 2.2* b->radius;
    int y1 = b->pos.screen_y + 6;
    int y2 = b->pos.screen_y - 6;

    for (int i=-1; i<=1; i++) {
        SDL_SetRenderDrawColor(ren, 255, 255, 230, 255);
        SDL_RenderDrawLine(ren, x1, y1+i, x2, y2+i);
    }
}
//SDL_SetRenderDrawColor(ren, 255, 255, 200, 255);

void check_earth_position(solar_system* sol, body* b) {

    if (sqrt(pow(b->pos.screen_x - SOLAR_X, 2) + pow(b->pos.screen_y - SOLAR_Y, 2)) < sol->sun.radius) {
        b->destroyed = true;
        // b->pos.x = b->pos.y = 0;
        //
        // sol->moon.pos.Gx = SOLAR_X;
        // sol->moon.pos.Gy = SOLAR_Y;
        // sol->moon.pos.x = sol->earth.pos.x - sol->moon.pos.x;
        // sol->moon.pos.y = sol->earth.pos.y - sol->moon.pos.y;
        // sol->moon.vel.x = sol->earth.vel.x - sol->moon.vel.x;
        // sol->moon.vel.y = sol->earth.vel.y - sol->moon.vel.y;
        // // sol->moon.pos.screen_x = sol->moon.pos.x / SF + sol->moon.pos.Gx;
        // // sol->moon.pos.screen_y = sol->moon.pos.y / SF + sol->moon.pos.Gy;
        // sol->moon.Gmass = S_MASS;
    }
}

void check_body_position(body* b, body* sun) {

    if (sqrt(pow(b->pos.screen_x - SOLAR_X, 2) + pow(b->pos.screen_y - SOLAR_Y, 2)) < sun->radius) {
        b->destroyed = true;
    }
}

void update_moon_gravitational_centre(solar_system* sol) {

    sol->moon.pos.Gx = sol->earth.pos.screen_x;
    sol->moon.pos.Gy = sol->earth.pos.screen_y;

    // if (sol->earth.destroyed) {
    //
    // }
}

void moon(body* b) {

    b->pos.screen_x += 50;
    b->pos.screen_y += 50;
}


