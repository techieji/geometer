#include "geometer.h"

void generate(char* filename) {
    FILE* file = fopen(filename, "w");
    for (struct UserObjectSeq* item = objects.head; item != NULL; item = item->next) {
        struct UserObject* obj = item->here;
        // TODO: add attributes
        switch (obj->type) {
            case OBJ_LINE:
                fprintf(file, "\\draw[] (%fpt, %fpt) -- (%fpt, %fpt);\n",
                        obj->p1x, obj->p1y, obj->p2x, obj->p2y);
                break;
            case OBJ_CIRCLE:
                float r = sqrtf(pow(obj->p1x - obj->p1y, 2) + pow(obj->p2x - obj->p2y, 2));
                fprintf(file, "\\draw[] (%fpt, %fpt) circle (%fpt);\n",
                        obj->p1x, obj->p1y, r);
                break;
        }
    }
    fclose(file);
}
