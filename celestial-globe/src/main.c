#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <libnova/libnova.h>

#define NUM_STARS 4150
#define STAR_DIST 10

typedef struct star_data {
    double ra;  // Right ascension in radians
    double dec; // Declination in radians
    char *name; // Star name (if available)
} star_data_t;

star_data_t stars[NUM_STARS];

int main(int argc, char *argv[]) {
    InitWindow(1366, 768, "Celestial Globe");

    Camera3D camera = {0};
    camera.position = (Vector3){0.0, 0.0, 0.0};
    camera.up = (Vector3){0.0, 1.0, 0.0};
    camera.target = (Vector3){1.0, 0.0, 0.0};
    camera.fovy = 60.0;
    camera.projection = CAMERA_PERSPECTIVE;
    FILE *star_file = fopen("FK6.txt", "r");
    DisableCursor();

    // Parse star data and load the array with more easily-drawn values
    char *line = NULL;
    int i = 0;
    size_t len;
    while (getline(&line, &len, star_file) > 0) {
        printf("%s\n", line);
        // Skip the first two columns, I just haven't removed them yet
        char *token = strtok(line, "|");
        token = strtok(NULL, "|");
        stars[i].name = strtok(NULL, "|");
        char *ra_str = strtok(NULL, "|");
        char *dec_str = strtok(NULL, "|");

        struct ln_hms ra_hms = {0};
        ra_hms.hours = (unsigned short)strtol(strtok(ra_str, " "), NULL, 10);
        ra_hms.minutes = (unsigned short)strtol(strtok(NULL, " "), NULL, 10);
        ra_hms.seconds = strtod(strtok(NULL, " "), NULL);
        stars[i].ra = ln_hms_to_rad(&ra_hms);

        struct ln_dms dec_dms = {0};
        long degrees = strtol(strtok(dec_str, " "), NULL, 10);
        if (degrees < 0)
            dec_dms.neg = 1;
        dec_dms.degrees = (unsigned short)abs((int)degrees);
        dec_dms.minutes = (unsigned short)strtol(strtok(NULL, " "), NULL, 10);
        dec_dms.seconds = strtod(strtok(NULL, " "), NULL);
        stars[i].dec = ln_dms_to_rad(&dec_dms);
        printf("Name: %s\n", stars[i].name);
        printf("\tRA: %f\n", stars[i].ra);
        printf("\tDec: %f\n", stars[i].dec);
        line = NULL;
        i++;
    }
    free(line);
    SetTargetFPS(144);

    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_FREE);
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        for (int i = 0; i < NUM_STARS; i++) {
            double x = STAR_DIST * cos(stars[i].dec) * cos(stars[i].ra);
            double z = STAR_DIST * cos(stars[i].dec) * sin(stars[i].ra);
            double y = STAR_DIST * sin(stars[i].dec);
            Vector3 pos = {x, y, z};
            DrawSphere(pos, 0.01f, RAYWHITE);
        }
        EndMode3D();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
