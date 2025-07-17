#pragma once
#include <stdio.h>
#include <SDL.h>
#include <stdbool.h>

#include "constants.h"

typedef struct vector {
    double x, y;
} Vector;

typedef struct pixel_coordinate {
    double x, y;
} PixelCoordinate;

typedef struct body {
    Vector vel;
    Vector pos;
    double mass;
    int radius;
} Body;

typedef struct solar_system {
    Body sun, mercury, venus, earth, mars, jupiter, saturn, uranus, neptune;
    Body* bodies[BODY_COUNT];
    bool zoom;
} SolarSystem;

typedef struct control_panel {
    int speed;
    double gravity;
} ControlPanel;

typedef enum speed {slow = 1, fast = 1000} speed;
typedef enum gravity {low = -2, normal = 0, high = 2} gravity;

SolarSystem* init_solar_system(void);
void draw_solar_system(SDL_Renderer* r, SolarSystem* sol);
PixelCoordinate get_screen_pos(Vector position);
void draw_body(SDL_Renderer* r, Body* b);
void update_orbit(Body* b, SolarSystem* sol, double rt, double g);
void update_orbits(SolarSystem* sol, ControlPanel* cp);
void get_control_input(ControlPanel* cp, SDL_Event e);
ControlPanel* init_control_panel(void);
