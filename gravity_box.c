#include "sol.h"
#include "gravity_box.h"

int main(void) {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Orbits",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("Window could not be created, SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        printf("Renderer could not be created, SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Event e;
    //bool isFullscreen = false;
    bool quit = false;

    SolarSystem* sol = init_solar_system();
    ControlPanel* panel = init_control_panel();

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN) {
                get_control_input(panel, e);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        check_for_bounces(sol, panel);
        update_orbits(sol, panel);
        draw_solar_system(renderer, sol, panel);

        SDL_RenderPresent(renderer);
    }

    free(sol);
    free(panel);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}

void get_control_input(ControlPanel* cp, SDL_Event e) {
    switch (e.key.keysym.sym) {
        case SDLK_ESCAPE:
            // quit
            break;
        case SDLK_f:
            // fullscreen
            break;
        case SDLK_RIGHT:
            if (cp->speed < fast) {
                cp->speed *= SPEED_INCREMENT;
            }
            break;
        case SDLK_LEFT:
            if (cp->speed > slow) {
                cp->speed /= SPEED_INCREMENT;
            }
            break;
        case SDLK_UP:
            if (cp->gravity < high) {
                cp->gravity += GRAVITY_INCREMENT;
            }
            break;
        case SDLK_DOWN:
            if (cp->gravity > low) {
                cp->gravity -= GRAVITY_INCREMENT;
            }
            break;
        case SDLK_RIGHTBRACKET:
            if (cp->zoom < zoomedIn) {
                cp->zoom += ZOOM_INCREMENT * pow(cp->zoom, 0.5);
            }
            break;
        case SDLK_LEFTBRACKET:
            if (cp->zoom > zoomedOut) {
                cp->zoom -= ZOOM_INCREMENT * pow(cp->zoom, 0.5);
            }
            break;
        case SDLK_SPACE:
            // reset
            break;
        default:
            break;
    }
}

ControlPanel* init_control_panel(void) {
    ControlPanel* panel = calloc(1, sizeof(ControlPanel));

    if (panel == NULL) {
        fprintf(stderr, "Error initialising control panel\n");
        exit(EXIT_FAILURE);
    }
    panel->speed = slow;
    panel->gravity = normal;
    panel->zoom = zoomedOut;
    return panel;
}



SolarSystem* init_solar_system(void) {
    SolarSystem* sol = calloc(1, sizeof(SolarSystem));

    if (sol == NULL) {
        fprintf(stderr, "Error initialising solar system\n");
        exit(EXIT_FAILURE);
    }

    sol->sun.mass = SUN_MASS;
    sol->sun.radius = SUN_RADIUS;

    sol->mercury.mass = MERCURY_MASS;
    sol->mercury.radius = MERCURY_RADIUS;
    sol->mercury.pos.x = MERCURY_POSITION;
    sol->mercury.vel.y = MERCURY_VELOCITY;

    sol->venus.mass = VENUS_MASS;
    sol->venus.radius = VENUS_RADIUS;
    sol->venus.pos.x = VENUS_POSITION;
    sol->venus.vel.y = VENUS_VELOCITY;

    sol->earth.mass = EARTH_MASS;
    sol->earth.radius = EARTH_RADIUS;
    sol->earth.pos.x = EARTH_POSITION;
    sol->earth.vel.y = EARTH_VELOCITY;

    sol->mars.mass = MARS_MASS;
    sol->mars.radius = MARS_RADIUS;
    sol->mars.pos.x = MARS_POSITION;
    sol->mars.vel.y = MARS_VELOCITY;

    sol->jupiter.mass = JUPITER_MASS;
    sol->jupiter.radius = JUPITER_RADIUS;
    sol->jupiter.pos.x = JUPITER_POSITION;
    sol->jupiter.vel.y = JUPITER_VELOCITY;

    sol->saturn.mass = SATURN_MASS;
    sol->saturn.radius = SATURN_RADIUS;
    sol->saturn.pos.x = SATURN_POSITION;
    sol->saturn.vel.y = SATURN_VELOCITY;

    sol->uranus.mass = URANUS_MASS;
    sol->uranus.radius = URANUS_RADIUS;
    sol->uranus.pos.x = URANUS_POSITION;
    sol->uranus.vel.y = URANUS_VELOCITY;

    sol->neptune.mass = NEPTUNE_MASS;
    sol->neptune.radius = NEPTUNE_RADIUS;
    sol->neptune.pos.x = NEPTUNE_POSITION;
    sol->neptune.vel.y = NEPTUNE_VELOCITY;

    sol->bodies[0] = &sol->sun;
    sol->bodies[1] = &sol->mercury;
    sol->bodies[2] = &sol->venus;
    sol->bodies[3] = &sol->earth;
    sol->bodies[4] = &sol->mars;
    sol->bodies[5] = &sol->jupiter;
    sol->bodies[6] = &sol->saturn;
    sol->bodies[7] = &sol->uranus;
    sol->bodies[8] = &sol->neptune;

    return sol;
}

void draw_solar_system(SDL_Renderer* r, SolarSystem* sol, ControlPanel* cp) {
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);

    for (int i=0; i<BODY_COUNT; i++) {
        //printf("Body = %d\n", i);
        draw_body(r, sol->bodies[i], cp);
    }
}

void draw_body(SDL_Renderer* r, Body* b, ControlPanel* cp) {
    PixelCoordinate screen_pos = get_screen_pos(b->pos, cp);
    int screen_y = (int) screen_pos.y;

    for (int y = -b->radius; y <= b->radius; y++) {
        long double dx = sqrt(b->radius * b->radius - y * y);
        int startX = (int)(screen_pos.x - dx);
        int endX = (int)(screen_pos.x + dx);
        SDL_RenderDrawLine(r, startX, screen_y + y, endX, screen_y + y);
    }
}

PixelCoordinate get_screen_pos(Vector position, ControlPanel* cp) {
    PixelCoordinate pixel;
    pixel.x = position.x*cp->zoom/SF + SCREEN_WIDTH/2;
    pixel.y = position.y*cp->zoom/SF + SCREEN_HEIGHT/2;
    return pixel;
}

void update_orbits(SolarSystem* sol, ControlPanel* cp) {
    // skip the sun
    for (int i=1; i<BODY_COUNT; i++) {
        update_orbit(sol->bodies[i], sol, cp->speed, pow(10, cp->gravity));
    }
}

void update_orbit(Body* b, SolarSystem* sol, double rt, double g) {

    double ax = 0.0, ay = 0.0;

    for (int i = 0; i < BODY_COUNT; i++) {
        Body* other = sol->bodies[i];

        if (other == b) continue;

        double dx = other->pos.x - b->pos.x;
        double dy = other->pos.y - b->pos.y;
        double dist_sq = dx * dx + dy * dy;

        // Avoid division by zero
        if (dist_sq < 1e5) continue;

        double dist = sqrt(dist_sq);
        double force = G * other->mass / (dist_sq * dist);  // Gm/r^3

        ax += force * dx;
        ay += force * dy;
    }

    b->vel.x += ax * DT * rt * g;
    b->vel.y += ay * DT * rt * g;

    b->pos.x += b->vel.x * DT * rt;
    b->pos.y += b->vel.y * DT * rt;
}

void check_for_bounces(SolarSystem* sol, ControlPanel* cp) {
    for (int i=0; i<BODY_COUNT; i++) {
        check_for_bounce(sol->bodies[i], cp);
    }
}

void check_for_bounce(Body* b, ControlPanel* cp) {
    PixelCoordinate screen_pos = get_screen_pos(b->pos, cp);

    if (screen_pos.x < 0 || screen_pos.x > SCREEN_WIDTH) {
        b->vel.x = -0.8 * b->vel.x;
    }
    if (screen_pos.y < 0 || screen_pos.x > SCREEN_HEIGHT) {
        b->vel.y = -0.8 * b->vel.y;
    }
}
