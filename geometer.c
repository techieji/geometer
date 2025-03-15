#include "geometer.h"

SDL_Window* window;
SDL_Renderer* renderer;

char buffer[256];
uint8_t cursor = 0;
enum OperationMode mode = MO_NORMAL;
bool locked = false;

struct IntermediatePoints* ip = NULL;
struct UserObjectList objects = { NULL, NULL };

bool mainloop(void) {
    // Render background, cursor, and prompt bar
    float x, y;
    SDL_SetRenderDrawColor(renderer, 255, 250, 245, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_GetMouseState(&x, &y);
    locked = lock(&x, &y);
    renderCursor(x, y);
    // TODO: rewrite corner_display as a more intelligent function (newline handling)
#define CORNER_DISPLAY(x) SDL_RenderDebugText(renderer, 7, HEIGHT - 15, x)
    if (strlen(buffer) == 0) {
        switch (mode) {
            case MO_LINE:
                CORNER_DISPLAY("-- LINE --");
                break;
            case MO_CIRCLE:
                CORNER_DISPLAY("-- CIRCLE -- ");
                break;
        }
    } else CORNER_DISPLAY(buffer);

    // Poll events
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_EVENT_QUIT: return false;
            case SDL_EVENT_KEY_DOWN:
                switch (e.key.key) {
                    case SDLK_BACKSPACE:
                        buffer[--cursor] = '\0';
                        break;
                    case SDLK_RETURN:
                        execute(buffer);
                        memset(buffer, '\0', sizeof(buffer));
                        break;
                    case SDLK_ESCAPE:
                        if (ip != NULL) {
                            free(ip->arr);
                            free(ip);
                            ip = NULL;
                        }
                        mode = MO_NORMAL;
                        memset(buffer, '\0', sizeof(buffer));
                }
                break;
            case SDL_EVENT_TEXT_INPUT:   // Place keys into command buffer
                if (e.text.text[0] == ':') mode = MO_COMMAND;
                if (mode == MO_COMMAND) cursor = SDL_strlcat(buffer, e.text.text, sizeof(buffer));
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (ip != NULL) addPoint(x, y);
                break;
        }
    }

    // TODO: Render user data
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (struct UserObjectSeq* item = objects.head; item != NULL; item = item->next)
        draw(item->here);

    struct UserObject* obj;
    switch (mode) {
        case MO_LINE:
            if ((obj = drawIntermediateLine(x, y)) != NULL) append(&objects, obj);
            break;
        case MO_CIRCLE:
            if ((obj = drawIntermediateCircle(x, y)) != NULL) append(&objects, obj);
            break;
    }
 
    // Display data and wait
    SDL_RenderPresent(renderer);
    SDL_Delay(10);
    return true;
}

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) return -1;
    window = SDL_CreateWindow("Geometer", WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, NULL);
    if (!window || !renderer) return -1;

    SDL_HideCursor();
    SDL_StartTextInput(window);
    while (mainloop());
    SDL_ShowCursor(); // Is this necessary?

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
