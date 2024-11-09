#pragma once

#include <stdio.h>
#include <SDL.h>
#include <unistd.h>
#include <stdbool.h>

#define COMPLETED_ORBIT 500
#define ANGLE_INC 2.0 * M_PI / COMPLETED_ORBIT
#define SOLAR_X 400
#define SOLAR_Y 300
#define SOLAR_RADIUS 25
#define EARTH_RADIUS 10
#define EARTH_ORBITAL_RADIUS 250
#define MOON_RADIUS 5
#define MOON_ORBITAL_RADIUS 50
#define EARTH_INC 0.0125
#define MOON_INC 0.15625
#define SLEEP_TIME 25000

typedef struct body {
    int x, y;
    int radius;
    int orb_rad;
    double angle;
} body;

typedef struct solar_system {
    body sun, earth, moon;
} solar_system;

typedef struct motion {
    int rate, scale, current, direction;
} motion;

typedef enum speed {slow = 0, medium = 10, fast = 20} speed;
typedef enum direction {forwards = 1, backwards = -1} direction;

void draw_circle(SDL_Renderer* renderer, body* b);
void update_earth_orbit(body* earth, motion* m);
void update_moon_orbit(body* earth, body* moon, motion* m);
void init_solar_system(solar_system* sol, motion* m);
void update_motion(SDL_Event* e, motion* m, bool* quit);
void update_render(SDL_Renderer* renderer, solar_system* sol, motion* m);
