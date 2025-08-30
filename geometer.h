#pragma once
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_mouse.h>

// Perhaps make these adjustable?
#define WIDTH 940
#define HEIGHT 540

enum OperationMode {
    MO_NORMAL,    // No default actions (TODO: add navigation)
    MO_COMMAND,   // Enters data into command buffer
    MO_DELETE,    // Allows you to delete objects (TODO: implement)
    // IMPORTANT NOTE: this also takes values that correspond to UserObjectTypes,
    // for the mode associated with creating them
};

struct UserObject {
    enum UserObjectType {
        OBJ_LINE = 100,    // This is 100 because it can be an OperationMode as well!
        OBJ_CIRCLE,
        //OBJ_BEZIER,
        //OBJ_POINT,
        OBJ_TEXT
    } type;
    struct {
        float p1x, p1y, p2x, p2y, p3x, p3y, p4x, p4y;
    };
    // DO NOT TOUCH CODE BEFORE HERE, some code relies on the struct layout
    char* text; // Used for OBJ_TEXT
    enum Color {
        COLOR_BLACK,
        COLOR_TRANS,
        COLOR_RED,
        COLOR_BLUE
    } color;
};

struct UserObjectSeq {
    struct UserObject* here;
    struct UserObjectSeq* next;
};

struct UserObjectList {
    struct UserObjectSeq* head;
    struct UserObjectSeq* tail;
};

struct IntermediatePoints {
    enum UserObjectType type;
    size_t size;  // Literal size of arr
    size_t filled_elems;
    float* arr;   // Alternating xs and ys
};

struct ObjectType {
    enum UserObjectType type;
    char* name;
    struct UserObject* (*intermediate)(float x, float y);
    void (*draw)(struct UserObject* obj);
    void (*persist)(struct UserObject* obj, FILE* file);
};

extern const struct ObjectType types[];

extern SDL_Renderer* renderer;
extern enum OperationMode mode;
extern struct IntermediatePoints* ip;
extern struct UserObjectList objects;
extern bool locked;
extern struct UserObject* locked_on;
extern char buffer[256];

/* External API of utility files */

// render_tools.c
void renderCursor(float x, float y);
void setupIP(enum UserObjectType type, int npoints);
struct UserObject* toUserObject(void);
void addPoint(float x, float y);
struct UserObject* drawIntermediate(enum UserObjectType type, float x, float y);
void append(struct UserObjectList* l, struct UserObject* obj);
void remove_obj(struct UserObjectList* l, struct UserObject* obj);   // rename? minor fix
void draw(struct UserObject* obj);

// execute.c
void execute(char* str);

// lock.c
bool lock(float* x, float* y);

// persist.c
void generate(char* filename);
