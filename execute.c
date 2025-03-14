#include "geometer.h"

void execute(char* str) {
    if (strcmp(str, ":line") == 0) {
        mode = MO_LINE;
        setupIP(OBJ_LINE, 2);
    } else if (strcmp(str, ":q") == 0) {
        exit(EXIT_SUCCESS);
    }
}
