#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xcb.h>
#include <math.h>
#include <xcb/xcb.h>

#define HEIGHT 512
#define WIDTH HEIGHT
#define REPETITIONS 100

cairo_surface_t *surface;
cairo_t *cr;

float dt = 0.1;

typedef struct {
    xcb_point_t start;
    float mag;
    float angle;
} vector_t;

/*
 * Express vector as y - y0 = m(x - x0) where m is tan(angle)
 *  (x-xc)^2+(y - yc)^2=r^2
 *  (x-xc)^2+(mx+b - yc)^2=r^2
 *  x^2-2xxc + xc^2 + (mx)^2+2mx(b-yc)+(b-yc)^2=r^2
 *  (m^2+1)x^2 - 2x(xc+m(b+yc)) + xc^2 + (b+yc)^2-r^2 = 0
 */
// It took way too long to remember how to do this properly
void calc_intersect(vector_t vector, xcb_arc_t arc, xcb_point_t *ret) {
    // Convert a vector into a line equation
    double m = tan(vector.angle);
    float intercept = vector.start.y - m * vector.start.x;
    printf("%fx + %f\n", m, intercept);
    // Center of the circle
    float xc = arc.x + (float)arc.width / 2;
    float yc = arc.y + (float)arc.height / 2;

    // Quadratic formula to calculate intersection
    double a = m * m + 1;
    double b = 2 * (m * (intercept - yc) - xc);
    double c = xc * xc + pow(intercept - yc, 2) - pow((float)arc.height / 2, 2);

    // Return a point
    if (cosf(vector.angle) > 0)
        ret->x = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
    else
        ret->x = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);

    ret->y = m * ret->x + intercept;
}

// Use to calculate a reflection after
float calc_reflection(float angle_in, xcb_arc_t arc, xcb_point_t intersection) {
  float xc = arc.x + (float)arc.width / 2;
  float yc = arc.y + (float)arc.height / 2;
  float normal = atan((intersection.y - yc) / (intersection.x - xc));

  return 2 * normal - angle_in;
}

int main(int argc, char *argv[]) {
    // Generate X system connection and get first screen
    xcb_connection_t *conn = xcb_connect(NULL, NULL);
    xcb_window_t window = xcb_generate_id(conn);
    const xcb_setup_t *setup = xcb_get_setup(conn);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    xcb_screen_t *screen = iter.data;
    uint32_t values[2] = {screen->black_pixel, 3};

    xcb_gcontext_t gcontext = xcb_generate_id(conn);
    xcb_create_gc(conn, gcontext, screen->root,
                  XCB_GC_FOREGROUND | XCB_GC_LINE_WIDTH, values);

    values[0] = screen->white_pixel;
    values[1] = XCB_EVENT_MASK_EXPOSURE;

    // Create an X window
    xcb_create_window(conn, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0,
                      WIDTH, HEIGHT, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual,
                      XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK, values);

    xcb_map_window(conn, window);
    xcb_flush(conn);

    xcb_generic_event_t *event;

    const xcb_property_notify_event_t *propNotify;
    xcb_arc_t arc = {
        .width = 600,
        .height = 600,
        .angle1 = 0,
        .angle2 = 360 << 6 // Angles are in 1/64th of a degree??
    };
    float height, width;
    float dist;
    xcb_point_t endPoint;
    while ((event = xcb_wait_for_event(conn))) {
        switch (event->response_type & ~0x80) {
        case XCB_EXPOSE:
            height = ((xcb_expose_event_t *)event)->height;
            width = ((xcb_expose_event_t *)event)->width;
            arc.x = (width - arc.width) / 2;
            arc.y = (height - arc.height) / 2;
            vector_t targetVector = {.start =
                                         {
                                             .x = width / 2 - 12,
                                             .y = height / 2 - 200,
                                         },
                                     .angle = M_PI_4};

            xcb_poly_arc(conn, window, gcontext, 1, &arc);
            xcb_flush(conn);
            values[0] = 1;
            values[1] = 0;
            xcb_change_gc(conn, gcontext, XCB_GC_LINE_WIDTH, values);
            for (int i = 0; i < REPETITIONS; i++) {
                calc_intersect(targetVector, arc, &endPoint);
                xcb_poly_line(conn, XCB_COORD_MODE_ORIGIN, window, gcontext, 2,
                              (xcb_point_t[2]){targetVector.start, endPoint});
                targetVector.start = endPoint;
                targetVector.angle = M_PI + calc_reflection(targetVector.angle, arc, endPoint);
                printf("%f\n", targetVector.angle);
            }
            printf("%d %d\n", endPoint.x, endPoint.y);
            xcb_flush(conn);

            break;
        default:
            break;
        }
        free(event);
    }
    // surface = cairo_xcb_surface_create(conn, screen->root, visual_iter.data,
    // WIDTH, HEIGHT); cr = cairo_create(surface); cairo_set_source_rgba(cr,
    // 0.0, 0.0, 0.0, 1.0); cairo_fill(cr);
    // // cairo_move_to(cr, (double)WIDTH / 2, (double)HEIGHT / 2);
    // cairo_arc(cr, (double)WIDTH / 2, (double)HEIGHT / 2, 200, 0, M_PI * 2);
    //
    // cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    //
    // cairo_stroke(cr);
    // pause();
    // cairo_surface_flush(surface);
    xcb_disconnect(conn);
    return 0;
}
