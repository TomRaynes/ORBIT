#include "sol.h"
#include "constants.h"

int main(void) {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("ORBIT",
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

    SDL_Cursor *crosshair = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    SDL_SetCursor(crosshair);
    SDL_Event e;
    SolarSystem* sol = init_solar_system();

    for (int body=sun; body<=neptune; body++) {
        sol->bodies[body]->sun = sol->bodies[sun];
    }
    load_all_textures(renderer, sol);
    ControlPanel* panel = init_control_panel();

    while (!panel->quit) {
        // get user input
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                panel->quit = true;
            }
            if (e.type == SDL_KEYDOWN) {
                get_keyboard_input(window, panel, sol, e);
            }
            get_mouse_input(&e, panel, sol);
        }
        // draw background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (!panel->pause) {
            update_orbits(sol, panel);
        }
        if (panel->tracked_body != NULL) {
            track_body(panel);
        }
        draw_solar_system(renderer, sol, panel);
        SDL_RenderPresent(renderer);
    }

    // free memory
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

void get_keyboard_input(SDL_Window* w, ControlPanel* cp, SolarSystem* sol, SDL_Event e) {
    switch (e.key.keysym.sym) {
        case SDLK_ESCAPE:
            cp->quit = true;
            break;
        case SDLK_f:
            cp->fullscreen = !cp->fullscreen;
            SDL_SetWindowFullscreen(w, cp->fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
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
                cp->gravity_changed = true;
            }
            break;
        case SDLK_DOWN:
            if (cp->gravity > low) {
                cp->gravity -= GRAVITY_INCREMENT;
                cp->gravity_changed = true;
            }
            break;
        case SDLK_r: // reset
            reset_solar_system(sol);
            reset_control_panel(cp);
            break;
        case SDLK_v: // toggle view mode
            rotate_view_mode(cp);
            break;
        case SDLK_SPACE: // pause
            cp->pause = !cp->pause;
            break;
        case SDLK_o: // toggle show orbit
            cp->show_orbit = !cp->show_orbit;
            break;
        case SDLK_q:
            randomise_solar_system(sol, cp);
            break;
        case SDLK_c:
            cp->show_cursor = !cp->show_cursor;
        SDL_ShowCursor(cp->show_cursor);
        default: break;
    }
}

void get_mouse_input(SDL_Event *event, ControlPanel* cp, SolarSystem* sol) {
    static bool left_pressed = false;
    static bool right_pressed = false;
    static int left_initial_x = 0, left_initial_y = 0;
    static int right_initial_y = 0;

    switch (event->type) {
        case SDL_MOUSEBUTTONDOWN:
            // if SHIFT + left click
            if (SDL_GetModState() & KMOD_SHIFT && event->button.button == SDL_BUTTON_LEFT) {
                Body* b = get_body_from_mouse_pos(sol, event->motion, cp);

                if (b != NULL) {
                    cp->tracked_body = b;
                    //cp->tracked_pos = get_screen_pos(b, cp, true, true);
                }
                else {
                    cp->tracked_body = NULL;
                    cp->offsetX = 0;
                    cp->offsetY = 0;
                }
                return;
            }
            if (event->button.button == SDL_BUTTON_LEFT) {
                cp->tracked_body = NULL;
                left_pressed = true;
                left_initial_x = event->button.x;
                left_initial_y = event->button.y;
            } else if (event->button.button == SDL_BUTTON_RIGHT) {
                right_pressed = true;
                right_initial_y = event->button.y;
            }
        break;

        case SDL_MOUSEBUTTONUP:
            if (event->button.button == SDL_BUTTON_LEFT) {
                left_pressed = false;
            } else if (event->button.button == SDL_BUTTON_RIGHT) {
                right_pressed = false;
            }
            cp->hover_body = NULL;
        break;

        case SDL_MOUSEMOTION:
            if (left_pressed) {
                int delta_x = event->motion.x - left_initial_x;
                int delta_y = event->motion.y - left_initial_y;
                cp->offsetX += delta_x / cp->zoom;
                cp->offsetY += delta_y / cp->zoom;
                // Update initial position to current so that deltas are incremental
                left_initial_x = event->motion.x;
                left_initial_y = event->motion.y;
            }
            if (right_pressed) {
                if (cp->hover_body == NULL) {
                    cp->hover_body = get_body_from_mouse_pos(sol, event->motion, cp);
                }
                int delta_y = event->motion.y - right_initial_y;
                double oldOffset = cp->offsetY;
                PixelCoordinate oldPos;

                if (cp->hover_body != NULL) {
                    oldPos = get_screen_pos(cp->hover_body, cp, true, true);
                }
                cp->angle += delta_y;

                if (cp->hover_body != NULL) {
                    cp->offsetY -= get_offset_correction(cp->hover_body, oldPos, cp);
                }
                if (cp->angle < 0 ) {
                    cp->angle = 0;
                    cp->offsetY = oldOffset;
                }
                if (cp->angle > 45) {
                    cp->angle = 45;
                    cp->offsetY = oldOffset;
                }
                right_initial_y = event->motion.y;
            }
            break;
        case SDL_MOUSEWHEEL:
            cp->zoom_level -= event->wheel.y;

            if (cp->zoom_level > 40) {
                cp->zoom_level = 40;
            }
            if (cp->zoom_level < -40) {
                cp->zoom_level = -40;
            }
            cp->zoom = pow(1.15, cp->zoom_level);
            break;

        default: break;
    }
}

double get_offset_correction(Body* b, PixelCoordinate oldPos, ControlPanel* cp) {
    PixelCoordinate pos = get_screen_pos(b, cp, true, true);
    return (pos.y - oldPos.y) / cp->zoom;
}

Body* get_body_from_mouse_pos(SolarSystem* sol, SDL_MouseMotionEvent mouse, ControlPanel* cp) {
    for (int body=neptune; body>=mercury; body--) {
        if (is_hovering_on_body(sol->bodies[body], mouse.x, mouse.y, cp)) {
            if (sol->bodies[body]->in_front) {
                return sol->bodies[body];
            }
        }
    }
    for (int body=mercury; body<=neptune; body++) {
        if (is_hovering_on_body(sol->bodies[body], mouse.x, mouse.y, cp)) {
            if (!sol->bodies[body]->in_front) {
                return sol->bodies[body];

            }
        }
    }
    return NULL;
}

void track_body(ControlPanel* cp) {
    PixelCoordinate pos = get_screen_pos(cp->tracked_body, cp, true, false);
    cp->offsetX = (SCREEN_WIDTH/2 - pos.x) / cp->zoom;
    cp->offsetY = (SCREEN_HEIGHT/2 - pos.y) / cp->zoom;
}

bool is_hovering_on_body(Body* b, int mouseX, int mouseY, ControlPanel* cp) {
    PixelCoordinate pos = get_screen_pos(b, cp, true, true);
    int radius = get_render_size(b, cp);
    bool alignedX = mouseX > pos.x - radius && mouseX < pos.x + radius;
    bool alignedY = mouseY > pos.y - radius && mouseY < pos.y + radius;
    return alignedX && alignedY;
}

void randomise_solar_system(SolarSystem* sol, ControlPanel* cp) {
    reset_solar_system(sol);
    reset_control_panel(cp);
    srand(time(NULL));

    for (int body=mercury; body<=neptune; body++) {
        randomise_body_position(sol->bodies[body]);
    }
}

void randomise_body_position(Body* b) {
    // set position
    double radius = sqrt((b->sun->pos.x-b->pos.x)*(b->sun->pos.x-b->pos.x) + (b->sun->pos.y-b->pos.y)*(b->sun->pos.y-b->pos.y));
    double rand_factor = 2 * radius / RAND_MAX;
    b->pos.x = rand() * rand_factor - radius;
    int direction = rand() - RAND_MAX/2 > 0 ? 1 : -1;
    b->pos.y = direction * sqrt(radius*radius - b->pos.x*b->pos.x);

    // set velocity
    double velocity = sqrt(b->vel.x*b->vel.x + b->vel.y*b->vel.y);
    double baseX = -b->pos.y;
    double baseY = b->pos.x;
    double magnitude = sqrt(baseX * baseX + baseY * baseY);

    b->vel.x = velocity * baseX / magnitude;
    b->vel.y = velocity * baseY / magnitude;

    // update rendering group
    b->in_front = is_in_front(b);

}

void translate_origin(const char* direction, ControlPanel* cp) {
    if (strcmp(direction, "right") == 0) {
        cp->offsetX += TRANSLATION_INCREMENT / cp->zoom;
    }
    else if (strcmp(direction, "left") == 0) {
        cp->offsetX -= TRANSLATION_INCREMENT / cp->zoom;
    }
    else if (strcmp(direction, "up") == 0) {
        cp->offsetY -= TRANSLATION_INCREMENT / cp->zoom;
    }
    else if (strcmp(direction, "down") == 0) {
        cp->offsetY += TRANSLATION_INCREMENT / cp->zoom;
    }
}

ControlPanel* init_control_panel(void) {
    ControlPanel* panel = calloc(1, sizeof(ControlPanel));

    if (panel == NULL) {
        fprintf(stderr, "Error initialising control panel\n");
        exit(EXIT_FAILURE);
    }
    reset_control_panel(panel);
    panel->pause = false;
    panel->view_mode = planet_view;
    panel->show_orbit = true;
    panel->fullscreen = false;
    panel->show_cursor = true;

    return panel;
}

void reset_control_panel(ControlPanel* panel) {
    panel->speed = slow;
    panel->gravity = normal;
    panel->zoom = 1;
    panel->zoom_level = zoomDefault;
    panel->angle = 25;
    panel->offsetX = panel->offsetY = 0;
    panel->tracked_body = NULL;
    panel->gravity_changed = false;
    panel->show_orbit = true;
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
        if (cp->show_orbit && !cp->gravity_changed) {
            draw_orbit(r, sol->bodies[body], cp, false);
        }

        if (!sol->bodies[body]->in_front) {
            draw_body_image(r, sol->bodies[body], cp, body==saturn);

            if (cp->show_orbit && !cp->gravity_changed) {
                draw_orbit_overlay(r, sol->bodies[body], cp);
            }
        }
    }
    draw_body_image(r, sol->bodies[sun], cp, false);

    //draw planets in front of sun
    for (int body=mercury; body<BODY_COUNT; body++) {

        if (cp->show_orbit && !cp->gravity_changed) {
            draw_orbit(r, sol->bodies[body], cp, true);
        }

        if (sol->bodies[body]->in_front) {
            draw_body_image(r, sol->bodies[body], cp, body==saturn);

            if (cp->show_orbit && !cp->gravity_changed) {
                draw_orbit_overlay(r, sol->bodies[body], cp);
            }
        }
    }
}

void draw_orbit(SDL_Renderer* r, Body* b, ControlPanel* cp, bool front) {
    PixelCoordinate body = get_screen_pos(b, cp, false, true);
    PixelCoordinate sun = get_screen_pos(b->sun, cp, false, true);
    // TODO: clean up draw orbit functions
    double sunX = sun.x;
    double sunY = sun.y;
    double dx = body.x - sunX;
    double dy = body.y - sunY;
    double orbit_radius = sqrt(dx*dx + dy*dy);
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
    PixelCoordinate body = get_screen_pos(b, cp, false, true);
    PixelCoordinate sun = get_screen_pos(b->sun, cp, false, true);
    double sunX = sun.x;
    double sunY = sun.y;
    double dx = body.x - sunX;
    double dy = body.y - sunY;
    double orbit_radius = sqrt(dx*dx + dy*dy);
    // angle between planet–sun and x-axis
    double bodyAngle = body.x > sunX ? atan(dy/dx) : atan(dy/dx) + M_PI;
    // planet–sun–planetSurface angle (surface at point that intersects the orbit line)
    double radiusAngle = 2*asin(0.5*get_render_size(b, cp) / orbit_radius);

    bool positive_dx = body.x > sunX;
    // initial start angle at 3D perceived intersection point of orbit line and planet
    double startAngle = positive_dx ? bodyAngle + radiusAngle : bodyAngle - radiusAngle;
    double nextAngle = positive_dx ? startAngle + M_PI / 180 : startAngle - M_PI / 180;
    double endAngle = positive_dx ? startAngle + M_PI / 2 : startAngle - M_PI / 2;
    SDL_SetRenderDrawColor(r, 100, 80, 80, 255);

    while ((positive_dx && nextAngle < endAngle) || (!positive_dx && nextAngle > endAngle)) {
        int startX = sunX + (int) (orbit_radius * cos(startAngle));
        int startY = sunY + (int) (orbit_radius * sin(startAngle) * get_squash_factor(cp->angle));
        int endX = sunX + (int) (orbit_radius * cos(nextAngle));
        int endY = sunY + (int) (orbit_radius * sin(nextAngle) * get_squash_factor(cp->angle));

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

PixelCoordinate get_screen_pos(Body* b, ControlPanel* cp, bool squash, bool offset) {
    PixelCoordinate pixel;
    double scale = get_pos_scale(b, cp);
    pixel.x = b->pos.x * cp->zoom / scale;
    pixel.y = b->pos.y * cp->zoom / scale;
    pixel.x += SCREEN_WIDTH/2;

    if (offset) {
        pixel.x += cp->offsetX*cp->zoom;
    }

    if (squash) {
        pixel.y *= get_squash_factor(cp->angle);
    }
    pixel.y += SCREEN_HEIGHT/2;

    if (offset) {
        pixel.y += cp->offsetY*cp->zoom;
    }
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

// Leap Frog method
void update_orbit(Body* b, SolarSystem* sol, double rt, double g) {
    double ax, ay;
    // calculate acceleration
    calculate_acceleration(b, sol, &ax, &ay);
    // half step velocity update
    b->vel.x += 0.5 * ax * DT * rt * g;
    b->vel.y += 0.5 * ay * DT * rt * g;
    // full step position
    b->pos.x += b->vel.x * DT * rt;
    b->pos.y += b->vel.y * DT * rt;
    // recalculate acceleration
    calculate_acceleration(b, sol, &ax, &ay);
    // half step velocity
    b->vel.x += 0.5 * ax * DT * rt * g;
    b->vel.y += 0.5 * ay * DT * rt * g;

    // update rendering group
    b->in_front = is_in_front(b);
}

void calculate_acceleration(Body* b, SolarSystem* sol, double* ax, double* ay) {
    *ax = 0.0;
    *ay = 0.0;

    for (int i = 0; i < BODY_COUNT; i++) {
        Body* other = sol->bodies[i];

        if (other == b) {
            continue;
        }
        double dx = other->pos.x - b->pos.x;
        double dy = other->pos.y - b->pos.y;
        double dist_sq = dx * dx + dy * dy;

        // avoid dividing by zero
        if (dist_sq < pow(10, -5)) {
            continue;
        }
        double dist = sqrt(dist_sq);
        double force = G * other->mass / (dist_sq * dist);

        *ax += force * dx;
        *ay += force * dy;
    }
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
        fprintf(stderr, "ERROR: Failed to create texture array\n");
        exit(EXIT_FAILURE);
    }
    char path[500];

    for (int i = 0; i < PERSPECTIVES; ++i) {
        snprintf(path, sizeof(path), "%s/%02d.png", directory, i + 1);
        SDL_Surface* surface = IMG_Load(path);

        if (!surface) {
            fprintf(stderr, "ERROR: Failed to load image %s: %s\n", path, IMG_GetError());
            exit(EXIT_FAILURE);
        }

        textures[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (!textures[i]) {
            fprintf(stderr, "ERROR: Failed to create texture from %s: %s\n", path, SDL_GetError());
            exit(EXIT_FAILURE);
        }
    }
    return textures;
}

void draw_body_image(SDL_Renderer* r, Body* b, ControlPanel* cp, bool isSaturn) {
    PixelCoordinate pos = get_screen_pos(b, cp, true, true);
    int base_radius = get_render_size(b, cp);
    int radius = isSaturn ? base_radius * RING_FACTOR : base_radius;
    pos.x -= radius;
    pos.y -= radius;
    SDL_Rect position = {pos.x, pos.y, 2*radius, 2*radius};
    SDL_Texture* texture = b->textures[(int) cp->angle];
    SDL_RenderCopy(r, texture, NULL, &position);
}

