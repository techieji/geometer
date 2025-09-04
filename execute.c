#include "geometer.h"

// Util functions

bool startsWith(char* string, char* prefix) {
    return strncmp(prefix, string, strlen(prefix)) == 0;
}

// TODO replace this with an actual language
void execute(char* str) {
    if (strcmp(str, ":line") == 0) {
        mode = OBJ_LINE;
        setupIP(OBJ_LINE, 2);
    } else if (strcmp(str, ":q") == 0) {
        exit(EXIT_SUCCESS);
    } else if (strcmp(str, ":circle") == 0) {
        mode = OBJ_CIRCLE;
        setupIP(OBJ_CIRCLE, 2);
    } else if (strcmp(str, ":bezier") == 0) {
        mode = OBJ_BEZIER;
        setupIP(OBJ_BEZIER, 4);
    } else if (strcmp(str, ":text") == 0) {
        mode = OBJ_TEXT;
        setupIP(OBJ_TEXT, 1);
    } else if (strcmp(str, ":arc") == 0) {
        mode = OBJ_ARC;
        setupIP(OBJ_ARC, 3);
    } else if (startsWith(str, ":w")) {
        generate("file.tex");    // TODO make this configurable!
    } else if (startsWith(str, ":d")) {
        mode = MO_DELETE;
    }
}

// combine into function above
void shortcut(char c) {
    switch (c) {
        case 'l':
            return execute(":line");
        case 'c':
            return execute(":circle");
        case 'b':
            return execute(":bezier");
        case 't':
            return execute(":text");
        case 'a':
            return execute(":arc");
        case 'd':
            mode = MO_DELETE;
            return;
    }
}
