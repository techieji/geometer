#include "geometer.h"
#define RADIUS 5

bool goodDistance(float p1x, float p1y, float p2x, float p2y) {
    return (sqrtf(pow(p1x - p2x, 2) + pow(p1y - p2y, 2))) < RADIUS;
}

bool lock(float* x, float* y) {
    float originalx = *x, originaly = *y;
    for (struct UserObjectSeq* seq = objects.head; seq != NULL; seq = seq->next) {
        struct UserObject* item = seq->here;
        // Point check
        if (goodDistance(item->p4x, item->p4y, *x, *y)) {
            *x = item->p4x; *y = item->p4y;
        } else if (goodDistance(item->p3x, item->p3y, *x, *y)) {
            *x = item->p3x; *y = item->p3y;
        } else if (goodDistance(item->p2x, item->p2y, *x, *y)) {
            *x = item->p2x; *y = item->p2y;
        } else if (goodDistance(item->p1x, item->p1y, *x, *y)) {
            *x = item->p1x; *y = item->p1y;
        }
        if ((*x != originalx) || (*y != originaly)) return true;
        // Element specific locking
        switch (item->type) {
            case OBJ_LINE: // FIXME (doesn't work)
                /*
                // There has to be a better way...
                float cx = *x - item->p1x;
                float cy = *y - item->p1y;
                float ax = item->p2x - item->p1x;
                float ay = item->p2y - item->p1y;
                float k  = ((cx * ax) + (cy * ay)) / (sqrtf(ax*ax + ay*ay) * sqrtf(cx*cx + cy*cy));
                float px = k*ax + item->p1x;
                float py = k*ay + item->p1y;
                if ((0 < k) && (k < 1)) {
                    *x = px;
                    *y = py;
                }*/
        }
        if ((*x != originalx) || (*y != originaly)) return true;
    }
    // Horizontal/Vertical Locking
    if (mode == MO_LINE && ip->filled_elems >= 2) {
        if (abs(*x - ip->arr[0]) < RADIUS) *x = ip->arr[0];
        if (abs(*y - ip->arr[1]) < RADIUS) *y = ip->arr[1];
    }

    return (*x != originalx) || (*y != originaly);
}
