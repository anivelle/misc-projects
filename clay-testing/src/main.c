#define MAX_CORNER_POINTS 30
#include "./clay_sfml_renderer.c"

#include <stdio.h>

#define MIN_FLOAT_ERR 0.05
#define WINDOW_DIMENSIONS {1920, 30}
const Clay_Color COLOR_WHITE = (Clay_Color){255, 255, 255, 255};
const Clay_Color COLOR_BLACK = (Clay_Color){0, 0, 0, 255};
const Clay_Color COLOR_BLUE = (Clay_Color){0, 0, 255, 255};

Clay_RenderCommandArray Create_Layout() {
    Clay_BeginLayout();
    CLAY({.layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                                .height = CLAY_SIZING_GROW(0)},
                     .padding = CLAY_PADDING_ALL(5),
                     .childGap = 10,
                     .layoutDirection = CLAY_LEFT_TO_RIGHT},
          .backgroundColor = COLOR_WHITE}) {
        CLAY({.layout = {.sizing = {.width = CLAY_SIZING_FIT(0),
                                    .height = CLAY_SIZING_GROW(0)},
                         .padding = CLAY_PADDING_ALL(0)},
              .cornerRadius = CLAY_CORNER_RADIUS(5), }) {
            CLAY_TEXT(
                CLAY_STRING(
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz The quick brown fox jumps over the lazy dog"),
                CLAY_TEXT_CONFIG(
                    {.fontSize = 16, .textColor = COLOR_BLACK, .fontId = 1}));
        }
        CLAY({.layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                                    .height = CLAY_SIZING_GROW(0)}},
              .backgroundColor = COLOR_BLUE}) {}
    }
    return Clay_EndLayout();
}

bool reinitializeClay = false;
void HandleClayErrors(Clay_ErrorData errorData) {
    // printf("%s", errorData.errorText.chars);
    if (errorData.errorType == CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED) {
        reinitializeClay = true;
        Clay_SetMaxElementCount(Clay_GetMaxElementCount() * 2);
    } else if (errorData.errorType ==
               CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED) {
        reinitializeClay = true;
        Clay_SetMaxMeasureTextCacheWordCount(
            Clay_GetMaxMeasureTextCacheWordCount() * 2);
    }
}

int main(int argc, char *argv[]) {
    sfContextSettings settings = {.antiAliasingLevel = 8};
    sfRenderWindow *window =
        sfRenderWindow_create((sfVideoMode){WINDOW_DIMENSIONS, 10}, "Test Window",
                              sfNone, sfWindowed, &settings);
    sfView *view = sfView_create();
    sfVector2u shape = sfRenderWindow_getSize(window);
    sfRenderWindow_setPosition(window, (sfVector2i){0, 0});

    uint32_t total_memory = Clay_MinMemorySize();
    Clay_Arena memory_arena = Clay_CreateArenaWithCapacityAndMemory(
        total_memory, malloc(total_memory));
    Clay_Initialize(memory_arena, (Clay_Dimensions){shape.x, shape.y},
                    (Clay_ErrorHandler){HandleClayErrors, 0});
    Clay_SFML_Initialize(window);
    sfFont *fonts[2];
    fonts[0] = sfFont_createFromFile("/usr/share/fonts/TTF/Roboto-Regular.ttf");
    fonts[1] = sfFont_createFromFile(
        "/usr/share/fonts/TTF/FantasqueSansMono-Regular.ttf");

    if (fonts[0] == NULL || fonts[1] == NULL) {
        printf("Font error\n");
    }
    Clay_SetMeasureTextFunction(Clay_SFML_MeasureText, fonts);

    // Shape related
    // shapedata_t data = {10, {100, 200}, {0, 0, 0, 0}};
    // sfShape *rounded_rect = sfShape_create(sfGetPointCountCallback,
    //                                        sfGetPointCallback, (void
    //                                        *)&data);
    // sfShape_update(rounded_rect);
    //
    // sfShape_setFillColor(rounded_rect, sfWhite);
    // sfRenderWindow_setPosition(window, (sfVector2i){100, 100});
    // sfCircleShape *circle = sfCircleShape_create();
    // sfCircleShape_setFillColor(circle, sfWhite);
    //
    // sfView_setSize(view, (sfVector2f){shape.x, shape.y});
    // sfRenderWindow_setView(window, view);
    // sfVector2f velocity = {0.01, 0.01};
    // sfVector2f position = {shape.x / 2.0, shape.y / 2.0};
    //
    // char increasing = 1;

    // Main loop
    while (sfRenderWindow_isOpen(window)) {
        sfEvent *event = (sfEvent *)malloc(sizeof(sfEvent));

        while (sfRenderWindow_pollEvent(window, event)) {
            if (event->type == sfEvtClosed)
                sfRenderWindow_close(window);
            if (event->type == sfEvtResized) {
                shape = sfRenderWindow_getSize(window);
                Clay_SetLayoutDimensions((Clay_Dimensions){shape.x, shape.y});
                sfView_setSize(view, (sfVector2f){shape.x, shape.y});
                sfView_setCenter(view, (sfVector2f){event->size.size.x / 2.0,
                                                    event->size.size.y / 2.0});
                sfRenderWindow_setView(window, view);
            }
        }
        Clay_RenderCommandArray commands = Create_Layout();
        Clay_SFML_Render(commands, fonts);

        // Making shapes
        // float circle_radius = 100.0;
        // sfCircleShape_setOrigin(circle,
        //                         (sfVector2f){circle_radius,
        //                         circle_radius});
        // sfCircleShape_setRadius(circle, circle_radius);
        //
        // sfCircleShape_setPosition(circle,
        //                           (sfVector2f){shape.x / 2.0, shape.y
        //                           / 2.0});
        //
        // sfShape_setPosition(rounded_rect, position);
        //
        // sfFloatRect rect_bounds = sfShape_getGlobalBounds(rounded_rect);
        // position.x += velocity.x;
        // position.y += velocity.y;
        // printf("Position: %f, %f\n", position.x + rect_bounds.size.x,
        // position.y + rect_bounds.size.y); printf("Rect Bounds Position:
        // %f, %f\n", rect_bounds.position.x, rect_bounds.position.y);
        // printf("Window size: %d, %d\n", shape.x, shape.y);

        // Updating sizes over time (animation)
        // if (data.size.x > 300)
        //     increasing = 0;
        // if (data.size.x < 90)
        //     increasing = 1;
        //
        // if (increasing)
        //     data.size.x += velocity.x;
        // else
        //     data.size.x -= velocity.x;
        // sfShape_update(rounded_rect);

        // Moving?? Bounds checking doesn't quite work
        // if ((rect_bounds.position.x + rect_bounds.size.x) - shape.x >
        //         MIN_FLOAT_ERR ||
        //     rect_bounds.position.x - 0 < MIN_FLOAT_ERR) {
        //     velocity.x *= -1;
        //     printf("Velocity: %f, %f\n", velocity.x, velocity.y);
        // }
        // if ((rect_bounds.position.y + rect_bounds.size.y) - shape.y >
        //         MIN_FLOAT_ERR ||
        //     rect_bounds.position.y - 0 < MIN_FLOAT_ERR) {
        //     velocity.y *= -1;
        // }
        // sfRenderWindow_drawShape(window, rounded_rect,
        // &sfRenderStates_default);

        // sfRenderWindow_drawCircleShape(window, circle,
        // &sfRenderStates_default);
        // sfRenderWindow_display(window);
    }

    Clay_SFML_Close();
    return EXIT_SUCCESS;
}
