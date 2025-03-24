#include "geometer.h"

void generate(char* filename) {
    FILE* file = fopen(filename, "w");
    for (struct UserObjectSeq* item = objects.head; item != NULL; item = item->next) {
        struct UserObject* obj = item->here;
        // TODO: add attributes
        switch (obj->type) {
            case OBJ_LINE:
                fprintf(file, "\\draw[] (%f, %f) -- (%f, %f);\n",
                        obj->p1x, obj->p1y, obj->p2x, obj->p2y);
                break;
            case OBJ_CIRCLE:
                fprintf(file, "\\draw[] (%f, %f) circle (%f);\n",
                        obj->p1x, obj->p1y, sqrtf(pow(obj->p1x - obj->p1x, 2) + pow(obj->p2x - obj->p2x, 2)));
                break;
        }
    }
    fclose(file);
}
