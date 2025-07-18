#pragma once
#include <stdio.h>
#include <SDL.h>
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
} ControlPanel;

typedef enum speed {slow = 1, fast = 1000} speed;
typedef enum gravity {low = -2, normal = 0, high = 2} gravity;
typedef enum zoom {zoomedOut = -2, zoomDefault = 0, zoomedIn = 20} zoom;
enum {sun = 0, mercury, venus, earth, mars, jupiter, saturn, uranus, neptune};
enum {true_distance, true_size, planet_view, RESET_VIEW};

SolarSystem* init_solar_system(void);
void draw_solar_system(SDL_Renderer* r, SolarSystem* sol, ControlPanel* cp);
PixelCoordinate get_screen_pos(Body* b, ControlPanel* cp);
void draw_body(SDL_Renderer* r, Body* b, ControlPanel* cp);
void update_orbit(Body* b, SolarSystem* sol, double rt, double g);
void update_orbits(SolarSystem* sol, ControlPanel* cp);
void get_control_input(ControlPanel* cp, SolarSystem* sol, SDL_Event e);
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
double get_orbit_radius(Body* b, ControlPanel* cp);

void check_for_bounce(Body* b, ControlPanel* cp);
void check_for_bounces(SolarSystem* sol, ControlPanel* cp);




