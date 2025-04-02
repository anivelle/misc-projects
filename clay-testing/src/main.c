#include "./clay_sfml_renderer.c"

#include <stdio.h>

#define MIN_FLOAT_ERR 0.05

int main(int argc, char *argv[]) {

    sfContextSettings settings = {.antiAliasingLevel=8};
    sfRenderWindow *window =
        sfRenderWindow_create((sfVideoMode){{800, 600}, 10}, "Test Window",
                              sfResize, sfWindowed, &settings);
    sfView *view = sfView_create();
    sfVector2u shape = sfRenderWindow_getSize(window);

    shapedata_t data = {10, {100, 200}, {0, 0, 0, 0}};
    sfShape *rounded_rect = sfShape_create(sfGetPointCountCallback,
                                           sfGetPointCallback, (void *)&data);
    sfShape_update(rounded_rect);

    sfShape_setFillColor(rounded_rect, sfWhite);
    sfRenderWindow_setPosition(window, (sfVector2i){100, 100});
    sfCircleShape *circle = sfCircleShape_create();
    sfCircleShape_setFillColor(circle, sfWhite);

    sfView_setSize(view, (sfVector2f){shape.x, shape.y});
    sfRenderWindow_setView(window, view);
    sfVector2f velocity = {0.01, 0.01};
    sfVector2f position = {shape.x / 2.0, shape.y / 2.0};

    char increasing = 1; 
  
    // Main loop
    while (sfRenderWindow_isOpen(window)) {
        sfEvent *event = (sfEvent *)malloc(sizeof(sfEvent));
        while (sfRenderWindow_pollEvent(window, event)) {
            if (event->type == sfEvtClosed)
                sfRenderWindow_close(window);
            if (event->type == sfEvtResized) {
                shape = sfRenderWindow_getSize(window);
                sfView_setSize(view, (sfVector2f){shape.x, shape.y});
                sfView_setCenter(view, (sfVector2f){event->size.size.x / 2.0,
                                                    event->size.size.y / 2.0});
                position = (sfVector2f){shape.x / 2.0, shape.y / 2.0};
                sfRenderWindow_setView(window, view);
            }
        }
        sfRenderWindow_clear(window, (sfColor){0, 0, 0, 255});
        float circle_radius = 100.0;
        sfCircleShape_setOrigin(circle,
                                (sfVector2f){circle_radius, circle_radius});
        sfCircleShape_setRadius(circle, circle_radius);

        sfCircleShape_setPosition(circle,
                                  (sfVector2f){shape.x / 2.0, shape.y / 2.0});

        sfShape_setPosition(rounded_rect, position);

        sfFloatRect rect_bounds = sfShape_getGlobalBounds(rounded_rect);
        // position.x += velocity.x;
        // position.y += velocity.y;
        // printf("Position: %f, %f\n", position.x + rect_bounds.size.x,
        // position.y + rect_bounds.size.y); printf("Rect Bounds Position: %f,
        // %f\n", rect_bounds.position.x, rect_bounds.position.y);
        // printf("Window size: %d, %d\n", shape.x, shape.y);

        if (data.size.x > 300)
          increasing = 0;
        if (data.size.x < 90)
          increasing = 1;

        if (increasing)
          data.size.x += velocity.x;
        else 
          data.size.x -= velocity.x;
        sfShape_update(rounded_rect);
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
        sfRenderWindow_drawShape(window, rounded_rect, &sfRenderStates_default);

        // sfRenderWindow_drawCircleShape(window, circle,
        // &sfRenderStates_default);
        sfRenderWindow_display(window);
    }

    Clay_SFML_Close();
    return EXIT_SUCCESS;
}
