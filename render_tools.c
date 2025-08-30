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
    // This part relies on the memory layout of UserObject
    // Copy the array after skipping the type element
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
    __builtin_unreachable();
}

void drawLine(struct UserObject* obj) {
    SDL_RenderLine(renderer, obj->p1x, obj->p1y, obj->p2x, obj->p2y);
}

void persistLine(struct UserObject* obj, FILE* file) {
    fprintf(file, "\\draw[] (%fpt, %fpt) -- (%fpt, %fpt);\n",
            obj->p1x, obj->p1y, obj->p2x, obj->p2y);
}

void renderArc(float r, float x, float y, float start_angle, float end_angle) {
    for (float t = start_angle; t < end_angle; t += 0.001)
        SDL_RenderPoint(renderer, r*cos(t) + x, r*sin(t) + y);
}

struct UserObject* drawIntermediateCircle(float x, float y) {
    switch (ip->filled_elems/2) {
        case 2: return toUserObject();
        case 1:
            // Draw X on center  (TODO factor out as render point)
            SDL_RenderLine(renderer, ip->arr[0]-3, ip->arr[1]-3, ip->arr[0]+3, ip->arr[1]+3);
            SDL_RenderLine(renderer, ip->arr[0]-3, ip->arr[1]+3, ip->arr[0]+3, ip->arr[1]-3);
            // Draw circle
            float r = sqrtf(pow(ip->arr[0] - x, 2) + pow(ip->arr[1] - y, 2));
            renderArc(r, ip->arr[0], ip->arr[1], 0, 2*M_PI);
        case 0: return NULL;
    }
    __builtin_unreachable();
}

void drawCircle(struct UserObject* obj) {
    float r = sqrtf(pow(obj->p1x - obj->p2x, 2) + pow(obj->p1y - obj->p2y, 2));
    renderArc(r, obj->p1x, obj->p1y, 0, 2*M_PI);
}

void persistCircle(struct UserObject* obj, FILE* file) {
    float r = sqrtf(pow(obj->p1x - obj->p2x, 2) + pow(obj->p1y - obj->p2y, 2));
    fprintf(file, "\\draw[] (%fpt, %fpt) circle (%fpt);\n", obj->p1x, obj->p1y, r);
}

struct UserObject* drawIntermediateArc(float x, float y) {
    switch (ip->filled_elems/2) {
        case 3: return toUserObject();
        case 2:
            // Draw X on center (TODO factor out)
            SDL_RenderLine(renderer, ip->arr[0]-3, ip->arr[1]-3, ip->arr[0]+3, ip->arr[1]+3);
            SDL_RenderLine(renderer, ip->arr[0]-3, ip->arr[1]+3, ip->arr[0]+3, ip->arr[1]-3);
            // Calculate angles
            float ax = ip->arr[2] - ip->arr[0], ay = ip->arr[3] - ip->arr[1],
                  bx = x - ip->arr[0], by = y - ip->arr[1];
            float start_angle = acosf((ax)/sqrtf(ax*ax + ay*ay));
            float end_angle = acosf((bx)/sqrtf(bx*bx + by*by));
            // Draw circle
            float r = sqrtf(ax*ax + ay*ay);
            renderArc(r, ip->arr[0], ip->arr[1], start_angle, end_angle);
            // TODO draw (dotted) line
            return NULL;
        case 1:
        case 0:
            return drawIntermediateCircle(x, y);
    }
    __builtin_unreachable();
}

// TODO finish arc

struct UserObject* drawIntermediateText(float x, float y) {  // Stupid and unintuitive interface FIXME
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
    __builtin_unreachable();
}

void drawText(struct UserObject* obj) {
    SDL_RenderDebugText(renderer, obj->p1x, obj->p1y, obj->text);
}

void persistText(struct UserObject* obj, FILE* file) {
    fprintf(file, "\\node[anchor=northwest] at (%fpt, %fpt) {%s};\n",
            obj->p1x, obj->p1y, obj->text);
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

void remove_obj(struct UserObjectList* l, struct UserObject* obj) {    // Never thought I'd have to implement LL methods...
    struct UserObjectSeq* prev = NULL;
    for (struct UserObjectSeq* seq = l->head; seq != NULL; seq = seq->next) {
        if (seq->here == obj) {
            if (prev == NULL) l->head = seq->next;
            else prev->next = seq->next;
            if (seq->next == NULL) l->tail = prev;
            free(seq->here);
            free(seq);
            return;
        }
    }
}

const struct ObjectType types[] = {
    { OBJ_LINE, "-- LINE --", drawIntermediateLine, drawLine, persistLine },
    { OBJ_CIRCLE, "-- CIRCLE --", drawIntermediateCircle, drawCircle, persistCircle },
    { OBJ_TEXT, "-- TEXT --", drawIntermediateText, drawText, persistText },
    { -100, NULL, NULL, NULL, NULL }       // Sentinel value?
};

struct UserObject* drawIntermediate(enum UserObjectType type, float x, float y) {
    for (int i = 0; i < sizeof(types); i++) {
        if (types[i].name == NULL) return NULL;
        if (types[i].type == type) return types[i].intermediate(x, y);
    }
    __builtin_unreachable();     // Maybe add a check here for weird types? For that matter:
                                 // TODO: better error handling
}

void draw(struct UserObject* obj) {
    if (obj == locked_on && mode == MO_DELETE)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    else
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 0; i < sizeof(types); i++)
        if (types[i].type == obj->type)
            return types[i].draw(obj);
}

void persist(struct UserObject* obj, FILE* file) {
    for (int i = 0; i < sizeof(types); i++)
        if (types[i].type == obj->type)
            return types[i].persist(obj, file);
}

void generate(char* filename) {
    FILE* file = fopen(filename, "w");
    for (struct UserObjectSeq* item = objects.head; item != NULL; item = item->next)
        persist(item->here, file);
    fclose(file);
}
