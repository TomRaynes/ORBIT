#pragma once

#include <stdio.h>
#include <SDL.h>
#include <unistd.h>
#include <stdbool.h>

#define COMPLETED_ORBIT 500
#define ANGLE_INC 2.0 * M_PI / COMPLETED_ORBIT
#define SOLAR_X 480
#define SOLAR_Y 300
#define SOLAR_RADIUS 25
#define EARTH_RADIUS 10
#define EARTH_ORBITAL_RADIUS 250
#define MOON_RADIUS 5
#define MOON_ORBITAL_RADIUS 50
#define EARTH_INC 0.0125
#define MOON_INC 0.15625
#define SLEEP_TIME 25000

#define DT 3600
#define SF 598400000
#define G 6.6743*pow(10, -11)
#define S_MASS 1.989*pow(10, 30)
#define E_MASS 5.972*pow(10, 29) // 5.972*pow(10, 24)
#define EARTH_POS_X 1.496*pow(10, 11)
#define EARTH_POS_Y 0
#define EARTH_VEL_X 0
#define EARTH_VEL_Y 29780
#define MOON_POS_X 3.833*pow(10, 10) // 3.833*pow(10, 8)
#define MOON_POS_Y 0
#define MOON_VEL_X 0
#define MOON_VEL_Y 30220 // 1022
// #define MARS_POS_X 1.496*pow(10, 11)
// #define MARS_POS_Y 0
// #define MARS_VEL_X 0
// #define MARS_VEL_Y 29780

#define BODY_COUNT 8

typedef struct velocity {
    double x, y;
} velocity;

typedef struct position {
    double x, y;
    int screen_x, screen_y;
    int Gx, Gy;
} position;

typedef struct body {
    velocity vel;
    position pos;
    bool destroyed;
    double Gmass;
    int radius;
} body;

typedef struct solar_system {
    body sun, earth, moon, mars, jupiter, saturn;
    body* bodies[BODY_COUNT];
    bool zoom;
} solar_system;

typedef struct motion {
    int rate, scale, current, direction;
} motion;

typedef enum speed {slow = -10, min_speed = -6, medium = 0, vel = 1, fast = 10} speed;
typedef enum direction {forwards = 1, backwards = -1} direction;

void update_orbit(body* b, solar_system* sol, double rt, double g);
void init_solar_system(solar_system* sol);
void draw_body(SDL_Renderer* ren, body* b);
void check_earth_position(solar_system* sol, body* b);
void update_moon_gravitational_centre(solar_system* sol);
void moon(body* b);
void update_moon_orbit(body* earth, body* b, double rt, double g);
void check_body_position(body* b, body* sun);
void draw_saturn(SDL_Renderer* ren, body* b);
void draw_jupiter(SDL_Renderer* ren, body* b);
