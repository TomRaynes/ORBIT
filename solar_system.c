#include "sol.h"
#include "constants.h"

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
    bool quit = false;

    SolarSystem* sol = init_solar_system();

    for (int body=sun; body<=neptune; body++) {
        sol->bodies[body]->sun = sol->bodies[sun];
    }
    load_all_textures(renderer, sol);
    ControlPanel* panel = init_control_panel();

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN) {
                get_control_input(renderer, panel, sol, e);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (!panel->pause) {
            update_orbits(sol, panel);
        }
        draw_solar_system(renderer, sol, panel);
        SDL_RenderPresent(renderer);
    }

    for (int body = sun; body<=neptune; body++) {
        free(sol->bodies[body]->textures);
    }
    free(sol);
    free(panel);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void load_all_textures(SDL_Renderer* r, SolarSystem* sol) {
    sol->bodies[sun]->textures = load_textures(r, "perspectives/sun");
    sol->bodies[mercury]->textures = load_textures(r, "perspectives/mercury");
    sol->bodies[venus]->textures = load_textures(r, "perspectives/venus");
    sol->bodies[earth]->textures = load_textures(r, "perspectives/earth");
    sol->bodies[mars]->textures = load_textures(r, "perspectives/mars");
    sol->bodies[jupiter]->textures = load_textures(r, "perspectives/jupiter");
    sol->bodies[saturn]->textures = load_textures(r, "perspectives/saturn_ring");
    sol->bodies[uranus]->textures = load_textures(r, "perspectives/uranus");
    sol->bodies[neptune]->textures = load_textures(r, "perspectives/neptune");
}

void get_control_input(SDL_Renderer* r, ControlPanel* cp, SolarSystem* sol, SDL_Event e) {
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
            if (cp->zoom_level < zoomedIn) {
                adjust_zoom(cp, "increment");
            }
            break;
        case SDLK_LEFTBRACKET:
            if (cp->zoom_level > zoomedOut) {
                adjust_zoom(cp, "decrement");
            }
            break;
        case SDLK_SPACE:
            reset_solar_system(sol);
            reset_control_panel(cp);
            break;
        case SDLK_t:
            rotate_view_mode(cp);
            reset_control_panel(cp);
            reset_solar_system(sol);
            break;
        case SDLK_p:
            cp->pause = !cp->pause;
            break;
        case SDLK_EQUALS:
            if (cp->angle > 0) {
                cp->angle--;
            }
            break;
        case SDLK_MINUS:
            if (cp->angle < 45) {
                cp->angle++;
            }
            break;
        case SDLK_a: translate_origin(sol, "left", cp); break;
        case SDLK_d: translate_origin(sol, "right", cp); break;
        case SDLK_w: translate_origin(sol, "forward", cp); break;
        case SDLK_s: translate_origin(sol, "backward", cp); break;
        default: break;
    }
}

void translate_origin(SolarSystem* sol, const char* direction, ControlPanel* cp) {
    if (strcmp(direction, "right") == 0) {
        translate_bodies(sol, right, cp);
        // sol->sun.pos.x += TRANSLATION_INCREMENT;
        // cp->originX = sol->sun.pos.x;
    }
    else if (strcmp(direction, "left") == 0) {
        translate_bodies(sol, left, cp);
        // sol->sun.pos.x -= TRANSLATION_INCREMENT;
        // cp->originX = sol->sun.pos.x;
    }
    else if (strcmp(direction, "forward") == 0) {
        //cp->originZ += TRANSLATION_INCREMENT;

        adjust_zoom(cp, "increment");
        //cp->originX += TRANSLATION_INCREMENT * ZOOM_INCREMENT;
    }
    else if (strcmp(direction, "backward") == 0) {
        //cp->originZ -= TRANSLATION_INCREMENT;
        adjust_zoom(cp, "decrement");
        //cp->originX -= TRANSLATION_INCREMENT * ZOOM_INCREMENT;

    }
    // SDL_Rect origin = { cp->originX, cp->originZ, SCREEN_WIDTH, SCREEN_HEIGHT };
    // SDL_RenderSetViewport(r, &origin);
}

void translate_bodies(SolarSystem* sol, int direction, ControlPanel* cp) {
    double increment = cp->view_mode == true_distance ? TD_TRANSLATION_INCREMENT : TRANSLATION_INCREMENT;
    for (int body=sun; body<=neptune; body++) {
        sol->bodies[body]->pos.x += direction * increment;
    }
}

double get_scale(Body* b, ControlPanel* cp) {
    switch (cp->view_mode) {
        case true_size: return b->dist_scale;
        case true_distance: return DISTANCE_SF;
        case planet_view: return b->planet_view_scale;
        default: return -1;
    }
}

ControlPanel* init_control_panel(void) {
    ControlPanel* panel = calloc(1, sizeof(ControlPanel));

    if (panel == NULL) {
        fprintf(stderr, "Error initialising control panel\n");
        exit(EXIT_FAILURE);
    }
    reset_control_panel(panel);
    panel->pause = true;
    panel->view_mode = planet_view;
    return panel;
}

void reset_control_panel(ControlPanel* panel) {
    panel->speed = slow;
    panel->gravity = normal;
    panel->zoom = 1;
    panel->zoom_level = zoomDefault;
    panel->angle = 25;
    panel->originX = SCREEN_WIDTH/2;
}

void rotate_view_mode(ControlPanel* cp) {
    cp->view_mode++;

    if (cp->view_mode == RESET_VIEW) {
        cp->view_mode = true_distance;
    }
}

SolarSystem* init_solar_system(void) {
    SolarSystem* sol = calloc(1, sizeof(SolarSystem));

    if (sol == NULL) {
        fprintf(stderr, "Error initialising solar system\n");
        exit(EXIT_FAILURE);
    }
    reset_solar_system(sol);
    return sol;
}

void reset_solar_system(SolarSystem* sol) {
    sol->sun.mass = SUN_MASS;
    sol->sun.radius = SUN_RADIUS;
    sol->sun.true_radius = SUN_TRUE_RADIUS;
    sol->sun.pv_radius = SUN_PV_RADIUS;
    sol->sun.dist_scale = sol->sun.planet_view_scale = SUN_DIST_SF;
    sol->sun.pos.x = 0;
    sol->sun.pos.y = 0;
    sol->sun.is_sun = true;

    sol->mercury.mass = MERCURY_MASS;
    sol->mercury.radius = MERCURY_RADIUS;
    sol->mercury.true_radius = MERCURY_TRUE_RADIUS;
    sol->mercury.pv_radius = MERCURY_PV_RADIUS;
    sol->mercury.dist_scale = MERCURY_DIST_SF;
    sol->mercury.planet_view_scale = MERCURY_PVD_SF;
    sol->mercury.pos.x = MERCURY_POSITION;
    sol->mercury.vel.y = MERCURY_VELOCITY;
    sol->mercury.pos.y = sol->mercury.vel.x = 0;
    sol->mercury.in_front = true;

    sol->venus.mass = VENUS_MASS;
    sol->venus.radius = VENUS_RADIUS;
    sol->venus.true_radius = VENUS_TRUE_RADIUS;
    sol->venus.pv_radius = VENUS_PV_RADIUS;
    sol->venus.dist_scale = VENUS_DIST_SF;
    sol->venus.planet_view_scale = VENUS_PVD_SF;
    sol->venus.pos.x = VENUS_POSITION;
    sol->venus.vel.y = VENUS_VELOCITY;
    sol->venus.pos.y = sol->venus.vel.x = 0;
    sol->venus.in_front = true;

    sol->earth.mass = EARTH_MASS;
    sol->earth.radius = EARTH_RADIUS;
    sol->earth.true_radius = EARTH_TRUE_RADIUS;
    sol->earth.pv_radius = EARTH_PV_RADIUS;
    sol->earth.dist_scale = EARTH_DIST_SF;
    sol->earth.planet_view_scale = EARTH_PVD_SF;
    sol->earth.pos.x = EARTH_POSITION;
    sol->earth.vel.y = EARTH_VELOCITY;
    sol->earth.pos.y = sol->earth.vel.x = 0;
    sol->earth.in_front = true;

    sol->mars.mass = MARS_MASS;
    sol->mars.radius = MARS_RADIUS;
    sol->mars.true_radius = MARS_TRUE_RADIUS;
    sol->mars.pv_radius = MARS_PV_RADIUS;
    sol->mars.dist_scale = MARS_DIST_SF;
    sol->mars.planet_view_scale = MARS_PVD_SF;
    sol->mars.pos.x = MARS_POSITION;
    sol->mars.vel.y = MARS_VELOCITY;
    sol->mars.pos.y = sol->mars.vel.x = 0;
    sol->mars.in_front = true;

    sol->jupiter.mass = JUPITER_MASS;
    sol->jupiter.radius = JUPITER_RADIUS;
    sol->jupiter.true_radius = JUPITER_TRUE_RADIUS;
    sol->jupiter.pv_radius = JUPITER_PV_RADIUS;
    sol->jupiter.dist_scale = JUPITER_DIST_SF;
    sol->jupiter.planet_view_scale = JUPITER_PVD_SF;
    sol->jupiter.pos.x = JUPITER_POSITION;
    sol->jupiter.vel.y = JUPITER_VELOCITY;
    sol->jupiter.pos.y = sol->jupiter.vel.x = 0;
    sol->jupiter.in_front = true;

    sol->saturn.mass = SATURN_MASS;
    sol->saturn.radius = SATURN_RADIUS;
    sol->saturn.true_radius = SATURN_TRUE_RADIUS;
    sol->saturn.pv_radius = SATURN_PV_RADIUS;
    sol->saturn.dist_scale = SATURN_DIST_SF;
    sol->saturn.planet_view_scale = SATURN_PVD_SF;
    sol->saturn.pos.x = SATURN_POSITION;
    sol->saturn.vel.y = SATURN_VELOCITY;
    sol->saturn.pos.y = sol->saturn.vel.x = 0;
    sol->saturn.in_front = true;

    sol->uranus.mass = URANUS_MASS;
    sol->uranus.radius = URANUS_RADIUS;
    sol->uranus.true_radius = URANUS_TRUE_RADIUS;
    sol->uranus.pv_radius = URANUS_PV_RADIUS;
    sol->uranus.dist_scale = URANUS_DIST_SF;
    sol->uranus.planet_view_scale = URANUS_PVD_SF;
    sol->uranus.pos.x = URANUS_POSITION;
    sol->uranus.vel.y = URANUS_VELOCITY;
    sol->uranus.pos.y = sol->uranus.vel.x = 0;
    sol->uranus.in_front = true;

    sol->neptune.mass = NEPTUNE_MASS;
    sol->neptune.radius = NEPTUNE_RADIUS;
    sol->neptune.true_radius = NEPTUNE_TRUE_RADIUS;
    sol->neptune.pv_radius = NEPTUNE_PV_RADIUS;
    sol->neptune.dist_scale = NEPTUNE_DIST_SF;
    sol->neptune.planet_view_scale = NEPTUNE_PVD_SF;
    sol->neptune.pos.x = NEPTUNE_POSITION;
    sol->neptune.vel.y = NEPTUNE_VELOCITY;
    sol->neptune.pos.y = sol->neptune.vel.x = 0;
    sol->neptune.in_front = true;

    // set colours
    sol->sun.col.r = SUN_COLOUR_R;
    sol->sun.col.g = SUN_COLOUR_G;
    sol->sun.col.b = SUN_COLOUR_B;
    sol->mercury.col.r = MERCURY_COLOUR_R;
    sol->mercury.col.g = MERCURY_COLOUR_G;
    sol->mercury.col.b = MERCURY_COLOUR_B;
    sol->venus.col.r = VENUS_COLOUR_R;
    sol->venus.col.g = VENUS_COLOUR_G;
    sol->venus.col.b = VENUS_COLOUR_B;
    sol->earth.col.r = EARTH_COLOUR_R;
    sol->earth.col.g = EARTH_COLOUR_G;
    sol->earth.col.b = EARTH_COLOUR_B;
    sol->mars.col.r = MARS_COLOUR_R;
    sol->mars.col.g = MARS_COLOUR_G;
    sol->mars.col.b = MARS_COLOUR_B;
    sol->jupiter.col.r = JUPITER_COLOUR_R;
    sol->jupiter.col.g = JUPITER_COLOUR_G;
    sol->jupiter.col.b = JUPITER_COLOUR_B;
    sol->saturn.col.r = SATURN_COLOUR_R;
    sol->saturn.col.g = SATURN_COLOUR_G;
    sol->saturn.col.b = SATURN_COLOUR_B;
    sol->uranus.col.r = URANUS_COLOUR_R;
    sol->uranus.col.g = URANUS_COLOUR_G;
    sol->uranus.col.b = URANUS_COLOUR_B;
    sol->neptune.col.r = NEPTUNE_COLOUR_R;
    sol->neptune.col.g = NEPTUNE_COLOUR_G;
    sol->neptune.col.b = NEPTUNE_COLOUR_B;

    sol->bodies[0] = &sol->sun;
    sol->bodies[1] = &sol->mercury;
    sol->bodies[2] = &sol->venus;
    sol->bodies[3] = &sol->earth;
    sol->bodies[4] = &sol->mars;
    sol->bodies[5] = &sol->jupiter;
    sol->bodies[6] = &sol->saturn;
    sol->bodies[7] = &sol->uranus;
    sol->bodies[8] = &sol->neptune;
}

void draw_solar_system(SDL_Renderer* r, SolarSystem* sol, ControlPanel* cp) {
    // draw planets behind sun
    for (int body=neptune; body>sun; body--) {
        draw_orbit(r, sol->bodies[body], cp, false);

        if (!sol->bodies[body]->in_front) {
            draw_body_image(r, sol->bodies[body], cp, body==saturn);
            draw_orbit_overlay(r, sol->bodies[body], cp);
        }
    }
    draw_body_image(r, sol->bodies[sun], cp, false);

    //draw planets in front of sun
    for (int body=mercury; body<BODY_COUNT; body++) {
        draw_orbit(r, sol->bodies[body], cp, true);

        if (sol->bodies[body]->in_front) {
            draw_body_image(r, sol->bodies[body], cp, body==saturn);
            draw_orbit_overlay(r, sol->bodies[body], cp);
        }
    }
}

void draw_body(SDL_Renderer* r, Body* b, ControlPanel* cp) {
    PixelCoordinate screen_pos = get_screen_pos(b, cp);
    int screen_y = (int) screen_pos.y;
    int radius = get_render_size(b, cp);

    for (int y=-radius; y<=radius; y++) {
        long double dx = sqrt(radius*radius - y * y);
        int startX = (int)(screen_pos.x - dx);
        int endX = (int)(screen_pos.x + dx);
        set_colour(r, b);
        SDL_RenderDrawLine(r, startX, screen_y + y, endX, screen_y + y);
    }
}

void draw_orbit(SDL_Renderer* r, Body* b, ControlPanel* cp, bool front) {
    double sunX = SCREEN_WIDTH/2;
    double sunY = SCREEN_HEIGHT/2;
    double dx = b->pos.x - sunX;
    double dy = b->pos.y - sunY;
    double orbit_radius = sqrt(dx*dx + dy*dy) * cp->zoom;

    switch (cp->view_mode) {
        case true_distance: orbit_radius /= DISTANCE_SF; break;
        case true_size: orbit_radius /= b->dist_scale; break;
        case planet_view: orbit_radius /= b->planet_view_scale; break;
        default: break;
    }
    int segments = 180;
    SDL_SetRenderDrawColor(r, 100, 80, 80, 255);

    for (int i=0; i<segments; i++) {
        double startAngle = front ?  2*M_PI * i / (2*segments) : -2*M_PI * i / (2*segments);
        double endAngle = front ? 2*M_PI * (i + 1) / (2*segments) : -2*M_PI * (i + 1) / (2*segments);

        int startX = sunX + (int) (orbit_radius * cos(startAngle));
        int startY = sunY + (int) (orbit_radius * sin(startAngle) * get_squash_factor(cp->angle));
        int endX = sunX + (int) (orbit_radius * cos(endAngle));
        int endY = sunY + (int) (orbit_radius * sin(endAngle) * get_squash_factor(cp->angle));

        SDL_RenderDrawLine(r, startX, startY, endX, endY);
    }
}

void draw_orbit_overlay(SDL_Renderer* r, Body* b, ControlPanel* cp) {
    double sunX = 0;
    double sunY = 0;
    double dx = b->pos.x - sunX;
    double dy = b->pos.y - sunY;
    double orbit_radius = sqrt(dx*dx + dy*dy) * cp->zoom / get_pos_scale(b, cp);
    // angle between planet–sun and x-axis
    double bodyAngle = b->pos.x > sunX ? atan(dy/dx) : atan(dy/dx) + M_PI;
    // planet–sun–planetSurface angle (surface at point that intersects the orbit line)
    double radiusAngle = 2*asin(0.5*get_render_size(b, cp) / orbit_radius);

    bool positive_dx = b->pos.x - sunX > 0;
    // initial start angle at 3D perceived intersection point of orbit line and planet
    double startAngle = positive_dx ? bodyAngle + radiusAngle : bodyAngle - radiusAngle;
    double nextAngle = positive_dx ? startAngle + M_PI / 180 : startAngle - M_PI / 180;
    double endAngle = positive_dx ? startAngle + M_PI / 2 : startAngle - M_PI / 2;
    SDL_SetRenderDrawColor(r, 100, 80, 80, 255);

    while ((positive_dx && nextAngle < endAngle) || (!positive_dx && nextAngle > endAngle)) {
        int startX = SCREEN_WIDTH/2 + (int) (orbit_radius * cos(startAngle));
        int startY = SCREEN_HEIGHT/2 + (int) (orbit_radius * sin(startAngle) * get_squash_factor(cp->angle));
        int endX = SCREEN_WIDTH/2 + (int) (orbit_radius * cos(nextAngle));
        int endY = SCREEN_HEIGHT/2 + (int) (orbit_radius * sin(nextAngle) * get_squash_factor(cp->angle));

        SDL_RenderDrawLine(r, startX, startY, endX, endY);
        startAngle = nextAngle;
        nextAngle = positive_dx ? nextAngle + M_PI / 180 : nextAngle - M_PI / 180;
    }
}

void adjust_zoom(ControlPanel* cp, const char* operation) {
    if (strcmp(operation, "increment") == 0) {
        cp->zoom_level += ZOOM_INCREMENT;
    }
    else if (strcmp(operation, "decrement") == 0) {
        cp->zoom_level -= ZOOM_INCREMENT;
    }
    else if (strcmp(operation, "reset") == 0) {
        cp->zoom_level = 0;
    }
    cp->zoom = pow(1.32, cp->zoom_level);
}

int get_render_size(Body* b, ControlPanel* cp) {
    switch (cp->view_mode) {
        case true_size: return b->true_radius * cp->zoom / RADIUS_SF;
        case true_distance: return b->radius * cp->zoom;
        case planet_view: return b->pv_radius * cp->zoom;
        default: return -1;
    }
}

PixelCoordinate get_screen_pos(Body* b, ControlPanel* cp) {
    PixelCoordinate pixel;
    double scale = get_scale(b, cp);
    pixel.x = b->pos.x * cp->zoom / scale;
    pixel.y = b->pos.y * cp->zoom / scale;

    if (!b->is_sun) {
        pixel.x += get_screen_pos(b->sun, cp).x - SCREEN_WIDTH/2;
    }
    pixel.x += SCREEN_WIDTH/2;
    pixel.y *= get_squash_factor(cp->angle);
    pixel.y += SCREEN_HEIGHT/2;
    return pixel;
}

double get_pos_scale(Body* b, ControlPanel* cp) {
    switch (cp->view_mode) {
        case true_size: return b->dist_scale;
        case true_distance: return DISTANCE_SF;
        case planet_view: return b->planet_view_scale;
        default: return -1;
    }
}

double get_squash_factor(double angle) {
    return 1 - fabs(angle)/45;
}

void update_orbits(SolarSystem* sol, ControlPanel* cp) {
    // skip the sun
    for (int body=mercury; body<BODY_COUNT; body++) {
        update_orbit(sol->bodies[body], sol, cp->speed, pow(10, cp->gravity));
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

    b->in_front = is_in_front(b);
}

bool is_in_front(Body* b) {
    double current_radius = sqrt(b->pos.x*b->pos.x + b->pos.y*b->pos.y);
    return -b->pos.y/current_radius < 0.6 || b->pos.y > 0;
}

void set_colour(SDL_Renderer* r, Body* b) {
    SDL_SetRenderDrawColor(r, b->col.r, b->col.g, b->col.b, PLANET_COLOUR_OPACITY);
}

SDL_Texture** load_textures(SDL_Renderer* renderer, const char* directory) {
    SDL_Texture** textures = malloc(PERSPECTIVES * sizeof(SDL_Texture*));

    if (textures == NULL) {
        fprintf(stderr, "ERROR: Failed to create texture array.\n");
        return NULL;
    }
    char path[500];

    for (int i = 0; i < PERSPECTIVES; ++i) {
        snprintf(path, sizeof(path), "%s/%02d.png", directory, i + 1);
        SDL_Surface* surface = IMG_Load(path);

        if (!surface) {
            fprintf(stderr, "ERROR: Failed to load image %s: %s\n", path, IMG_GetError());
            textures[i] = NULL;
            continue;
        }

        textures[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (!textures[i]) {
            fprintf(stderr, "ERROR: Failed to create texture from %s: %s\n", path, SDL_GetError());
        }
    }
    return textures;
}

void draw_body_image(SDL_Renderer* r, Body* b, ControlPanel* cp, bool isSaturn) {
    PixelCoordinate pos = get_screen_pos(b, cp);
    int base_radius = get_render_size(b, cp);
    int radius = isSaturn ? base_radius * RING_FACTOR : base_radius;
    pos.x -= radius;
    pos.y -= radius;
    SDL_Rect position = {pos.x, pos.y, 2*radius, 2*radius};
    SDL_Texture* texture = b->textures[(int) cp->angle];
    SDL_RenderCopy(r, texture, NULL, &position);
}

