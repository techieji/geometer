#include "geometer.h"

void renderCursor(float x, float y) {
    if (locked) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    else SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderLine(renderer, x, y + 3, x, y - 3);
    SDL_RenderLine(renderer, x + 3, y, x - 3, y);
    SDL_RenderLine(renderer, x, y - 8, x, y - 14);
    SDL_RenderLine(renderer, x, y + 8, x, y + 14);
    SDL_RenderLine(renderer, x - 8, y, x - 14, y);
    SDL_RenderLine(renderer, x + 8, y, x + 14, y);
}

void setupIP(enum UserObjectType type, int npoints) {
    ip = malloc(sizeof(struct IntermediatePoints));
    ip->type = type;
    ip->size = 2 * npoints;
    ip->filled_elems = 0;
    ip->arr = malloc(sizeof(float));
}

struct UserObject* toUserObject(void) {
    struct UserObject* obj = malloc(sizeof(struct UserObject));
    obj->type = ip->type;
    memcpy((void*)obj + sizeof(enum UserObjectType), ip->arr, ip->size * sizeof(float));
    ip->filled_elems = 0;
    free(ip->arr);
    ip->arr = malloc(sizeof(float) * ip->size);
    return obj;
}

void addPoint(float x, float y) {
    ip->arr[ip->filled_elems++] = x;
    ip->arr[ip->filled_elems++] = y;
}

struct UserObject* drawIntermediateLine(float x, float y) {
    switch (ip->filled_elems/2) {
        case 2: return toUserObject();
        case 1: SDL_RenderLine(renderer, ip->arr[0], ip->arr[1], x, y);
        case 0: return NULL;
    }
}

void drawCircle(float r, float x, float y) {
    for (float t = 0; t < 2*M_PI; t += 0.001)
        SDL_RenderPoint(renderer, r*cos(t) + x, r*sin(t) + y);
 }

struct UserObject* drawIntermediateCircle(float x, float y) {
    switch (ip->filled_elems/2) {
        case 2: return toUserObject();
        case 1:
            // Draw X on center
            SDL_RenderLine(renderer, ip->arr[0]-3, ip->arr[1]-3, ip->arr[0]+3, ip->arr[1]+3);
            SDL_RenderLine(renderer, ip->arr[0]-3, ip->arr[1]+3, ip->arr[0]+3, ip->arr[1]-3);
            // Draw circle
            float r = sqrtf(pow(ip->arr[0] - x, 2) + pow(ip->arr[1] - y, 2));
            drawCircle(r, ip->arr[0], ip->arr[1]);
        case 0: return NULL;
    }
}

void append(struct UserObjectList* l, struct UserObject* obj) {
    if (l->head == NULL) {
        l->head = l->tail = malloc(sizeof(struct UserObjectSeq));
        l->head->here = obj;
        l->head->next = NULL;
    } else {
        l->tail->next = malloc(sizeof(struct UserObjectSeq));
        l->tail->next->here = obj;
        l->tail->next->next = NULL;
        l->tail = l->tail->next;
    }
}

void draw(struct UserObject* obj) {
    switch (obj->type) {
        case OBJ_LINE:
            SDL_RenderLine(renderer, obj->p1x, obj->p1y, obj->p2x, obj->p2y);
            return;
        case OBJ_CIRCLE:
            float r = sqrtf(pow(obj->p1x - obj->p2x, 2) + pow(obj->p1y - obj->p2y, 2));
            drawCircle(r, obj->p1x, obj->p1y);
            return;
    }
}
