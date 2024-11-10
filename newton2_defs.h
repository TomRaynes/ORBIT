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

#define DT 3600*24
#define SF 598400000
#define G 6.6743*pow(10, -11)
#define S_MASS 1.989*pow(10, 30)
#define EARTH_POS_X 1.496*pow(10, 11)
#define EARTH_POS_Y 0
#define EARTH_VEL_X 0
#define EARTH_VEL_Y 29780

typedef struct velocity {
    double x, y;
} velocity;

typedef struct position {
    double x, y;
    int screen_x, screen_y;
} position;

typedef struct body {
    velocity vel;
    position pos;
    int radius;
} body;

typedef struct solar_system {
    body sun, earth, moon;
} solar_system;

typedef struct motion {
    int rate, scale, current, direction;
} motion;

typedef enum speed {slow = 0, medium = 10, fast = 20} speed;
typedef enum direction {forwards = 1, backwards = -1} direction;

//void update_orbit(body* b);
void update_orbit(double* x, double* y, double* vx, double* vy);
void draw_sun(SDL_Renderer* renderer);
void draw_earth(SDL_Renderer* renderer, int X, int Y);
//void screen(solar_system* sol, int* X, int* Y);
void screen(int* X, int* Y, double* x, double* y);
