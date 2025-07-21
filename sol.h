#pragma once
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>

#define BODY_COUNT 9

typedef struct vector {
    double x, y;
} Vector;

typedef struct pixel_coordinate {
    double x, y;
} PixelCoordinate;

typedef struct colour {
    int r, g, b, a;
} Colour;

typedef struct body {
    Vector vel;
    Vector pos;
    double mass;
    Colour col;
    double radius; // for true distance
    double true_radius; // for true size
    double pv_radius; // for planet view
    double dist_scale; // specific scale for true size view mode
    double planet_view_scale; // specific scale for planet view mode
    bool in_front;
    bool is_sun;
    SDL_Texture** textures;
    struct body* sun;
} Body;

typedef struct solar_system {
    Body sun, mercury, venus, earth, mars, jupiter, saturn, uranus, neptune;
    Body* bodies[BODY_COUNT];
    bool zoom;
} SolarSystem;

typedef struct control_panel {
    int speed;
    double gravity;
    double zoom_level;
    double zoom;
    bool pause;
    int view_mode;
    double angle;
    double offsetX, offsetY;
    bool show_orbit;
    bool fullscreen;
    bool show_cursor;
    bool quit;
    Body* hover_body;
    Body* tracked_body;
    bool gravity_changed;
} ControlPanel;

typedef enum speed {slow = 1, fast = 100} speed;
typedef enum gravity {low = -2, normal = 0, high = 2} gravity;
typedef enum zoom {zoomedOut = -2, zoomDefault = 0, zoomedIn = 20} zoom;
enum {sun = 0, mercury, venus, earth, mars, jupiter, saturn, uranus, neptune};
enum {true_distance, true_size, planet_view, RESET_VIEW};
enum {left = -1, up = -1, right = 1, down = 1};

SolarSystem* init_solar_system(void);
void draw_solar_system(SDL_Renderer* r, SolarSystem* sol, ControlPanel* cp);
PixelCoordinate get_screen_pos(Body* b, ControlPanel* cp, bool squash, bool offset);
void draw_body(SDL_Renderer* r, Body* b, ControlPanel* cp);
void update_orbit(Body* b, SolarSystem* sol, double rt, double g);
void update_orbits(SolarSystem* sol, ControlPanel* cp);
void get_keyboard_input(SDL_Window* w, ControlPanel* cp, SolarSystem* sol, SDL_Event e);
ControlPanel* init_control_panel(void);
void reset_solar_system(SolarSystem* sol);
void reset_control_panel(ControlPanel* panel);
void set_colour(SDL_Renderer* r, Body* b);
int get_render_size(Body* b, ControlPanel* cp);
void adjust_zoom(ControlPanel* cp, const char* operation);
void rotate_view_mode(ControlPanel* cp);
double get_squash_factor(double angle);
void draw_orbit(SDL_Renderer* r, Body* b, ControlPanel* cp, bool front);
bool is_in_front(Body* b);
void draw_orbit_overlay(SDL_Renderer* r, Body* b, ControlPanel* cp);
double get_pos_scale(Body* b, ControlPanel* cp);
void draw_body_image(SDL_Renderer* r, Body* b, ControlPanel* cp, bool isSaturn);
SDL_Texture** load_textures(SDL_Renderer* renderer, const char* directory);
void load_all_textures(SDL_Renderer* r, SolarSystem* sol);
void translate_origin(const char* direction, ControlPanel* cp);
void get_mouse_input(SDL_Event *event, ControlPanel* cp, SolarSystem* sol);
void randomise_body_position(Body* b);
void randomise_solar_system(SolarSystem* sol, ControlPanel* cp);
void calculate_acceleration(Body* b, SolarSystem* sol, double* ax, double* ay);
bool is_hovering_on_body(Body* b, int mouseX, int mouseY, ControlPanel* cp);
Body* get_body_from_mouse_pos(SolarSystem* sol, SDL_MouseMotionEvent mouse, ControlPanel* cp);
double get_offset_correction(Body* b, PixelCoordinate oldPos, ControlPanel* cp);
void track_body(ControlPanel* cp);
