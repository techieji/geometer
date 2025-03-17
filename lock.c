#include "geometer.h"
#define RADIUS 10

bool goodDistance(float p1x, float p1y, float p2x, float p2y) {
    return (sqrtf(pow(p1x - p2x, 2) + pow(p1y - p2y, 2))) < RADIUS;
}

void lockPoint(struct UserObject* item, float* x, float *y) {
    if (goodDistance(item->p4x, item->p4y, *x, *y))      { *x = item->p4x; *y = item->p4y; }
    else if (goodDistance(item->p3x, item->p3y, *x, *y)) { *x = item->p3x; *y = item->p3y; }
    else if (goodDistance(item->p2x, item->p2y, *x, *y)) { *x = item->p2x; *y = item->p2y; }
    else if (goodDistance(item->p1x, item->p1y, *x, *y)) { *x = item->p1x; *y = item->p1y; }
}

void lockLine(struct UserObject* item, float* x, float *y) {
    // Fairly simple formula using dot product; idk if there's a simpler way
    float cx = *x - item->p1x;
    float cy = *y - item->p1y;
    float ax = item->p2x - item->p1x;
    float ay = item->p2y - item->p1y;
    float k  = (cx*ax + cy*ay) / (ax*ax + ay*ay);
    float px = item->p1x + k*ax;
    float py = item->p1y + k*ay;
    if ((0 < k) && (k < 1) && goodDistance(px, py, *x, *y)) {
        *x = px;
        *y = py;
    }
}

void lockCircle(struct UserObject* item, float* x, float* y) {
    float d = sqrtf(pow(*x - item->p1x, 2) + pow(*y - item->p1y, 2));   // TODO: avoid recomputing everytime (add field)
    float r = sqrtf(pow(item->p2x - item->p1x, 2) + pow(item->p2y - item->p1y, 2));
    if (abs(d - r) < RADIUS) {
        *x = r / d * (*x - item->p1x) + item->p1x;
        *y = r / d * (*y - item->p1y) + item->p1y;
    }
}

bool _lock(float* x, float* y) {
    float originalx = *x, originaly = *y;
    for (struct UserObjectSeq* seq = objects.head; seq != NULL; seq = seq->next) {
        struct UserObject* item = seq->here;
        // Point check
        lockPoint(item, x, y);
        if ((*x != originalx) || (*y != originaly)) continue;
        // Element specific locking
        switch (item->type) {
            case OBJ_LINE: lockLine(item, x, y); break;
            case OBJ_CIRCLE: lockCircle(item, x, y); break;
       }
        if ((*x != originalx) || (*y != originaly)) continue;
    }
    // Horizontal/Vertical Locking
    if (mode == MO_LINE && ip->filled_elems >= 2) {
        if (abs(*x - ip->arr[0]) < RADIUS) *x = ip->arr[0];
        if (abs(*y - ip->arr[1]) < RADIUS) *y = ip->arr[1];
    }

    return (*x != originalx) || (*y != originaly);
}

bool lock(float* x, float* y) {
    // There has to be a better way of implementing intersection locking...
    bool res = false;
    for (int i = 0; i < 10; i++) res = res | _lock(x, y);
    return res;
}
