#include "geometer.h"

bool startsWith(char* string, char* prefix) {
    return strncmp(prefix, string, strlen(prefix)) == 0;
}

void execute(char* str) {
    if (strcmp(str, ":line") == 0) {
        mode = OBJ_LINE;
        setupIP(OBJ_LINE, 2);
    } else if (strcmp(str, ":q") == 0) {
        exit(EXIT_SUCCESS);
    } else if (strcmp(str, ":circle") == 0) {
        mode = OBJ_CIRCLE;
        setupIP(OBJ_CIRCLE, 2);
    } else if (strcmp(str, ":text") == 0) {
        mode = OBJ_TEXT;
        setupIP(OBJ_TEXT, 1);
    } else if (startsWith(str, ":persist")) {
        generate("file.tex");
    }
}
