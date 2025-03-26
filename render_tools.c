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

void drawLine(struct UserObject* obj) {
    SDL_RenderLine(renderer, obj->p1x, obj->p1y, obj->p2x, obj->p2y);
}

void renderCircle(float r, float x, float y) {
    for (float t = 0; t < 2*M_PI; t += 0.001)
        SDL_RenderPoint(renderer, r*cos(t) + x, r*sin(t) + y);
}

void drawCircle(struct UserObject* obj) {    // TODO: move below intermediate circle fn
    float r = sqrtf(pow(obj->p1x - obj->p2x, 2) + pow(obj->p1y - obj->p2y, 2));
    renderCircle(r, obj->p1x, obj->p1y);
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
            renderCircle(r, ip->arr[0], ip->arr[1]);
        case 0: return NULL;
    }
}

struct UserObject* drawIntermediateText(float x, float y) {
    switch (ip->filled_elems/2) {
        case 1:
            struct UserObject* obj = toUserObject();
            obj->text = malloc(256 * sizeof(char));
            memcpy(obj->text, buffer, sizeof(buffer));
            memset(buffer, '\0', sizeof(buffer));
            return obj;
        case 0:
            SDL_RenderDebugText(renderer, x, y, buffer);
            return NULL;
    }
}

void drawText(struct UserObject* obj) {
    SDL_RenderDebugText(renderer, obj->p1x, obj->p1y, obj->text);
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

const struct ObjectType types[] = {
    { OBJ_LINE, "-- LINE --", drawIntermediateLine, drawLine },
    { OBJ_CIRCLE, "-- CIRCLE --", drawIntermediateCircle, drawCircle },
    { OBJ_TEXT, "-- TEXT --", drawIntermediateText, drawText },
    { -100, NULL, NULL, NULL }
};

struct UserObject* drawIntermediate(enum UserObjectType type, float x, float y) {
    for (int i = 0; i < sizeof(types); i++) {
        if (types[i].name == NULL) return NULL;
        if (types[i].type == type) return types[i].intermediate(x, y);
    }
}

void draw(struct UserObject* obj) {
    for (int i = 0; i < sizeof(types); i++)
        if (types[i].type == obj->type)
            return types[i].draw(obj);
}
