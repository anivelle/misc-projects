#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <libnova/libnova.h>

// #define NUM_STARS 118218
#define STAR_DIST 50
#define MIN_RADIUS 0.05

typedef struct star_data {
    double ra;  // Right ascension in radians
    double dec; // Declination in radians
    double vis_mag;
    char *name; // Star name (if available)
} star_data_t;

typedef struct render_data {
  char render; 
  star_data_t *data;
} render_data_t;

render_data_t render_table[1 << 17];
// star_data_t stars[NUM_STARS];

int main(int argc, char *argv[]) {
    InitWindow(1366, 768, "Celestial Globe");

    Camera3D camera = {0};
    camera.position = (Vector3){0.0, 0.0, 0.0};
    camera.up = (Vector3){0.0, 1.0, 0.0};
    camera.target = (Vector3){1.0, 0.0, 0.0};
    camera.fovy = 60.0;
    camera.projection = CAMERA_PERSPECTIVE;
    FILE *star_file = fopen("hip_main.dat", "r");
    FILE *orion = fopen("orion.csv", "r");
    FILE *ursa_maj = fopen("ursa_major.csv", "r");
    DisableCursor();

    // Parse star data and load the array with more easily-drawn values
    char *line = NULL;
    int i = 0;
    size_t len;

    while (getline(&line, &len, orion) > 0) {
        unsigned index = (unsigned)strtol(line, NULL, 10);
        // printf("%b\n", index & mask);
        // printf("%d", hash_table[hash(index)]);
        render_table[index].render = 1;
        free(line);
        line = NULL;
    }

    while (getline(&line, &len, ursa_maj) > 0) {
        unsigned index = (unsigned)strtol(line, NULL, 10);
        // printf("%b\n", index & mask);
        // printf("%d", hash_table[hash(index)]);
        render_table[index].render = 1;
        free(line);
        line = NULL;
    }

    while (getline(&line, &len, star_file) > 0) {
        // Skip the second column, I just haven't removed them yet
        star_data_t *star = malloc(sizeof(star_data_t));
        char *token = strtok(line, "|");
        unsigned id = (unsigned)strtol(token, NULL, 10);
        render_table[id].data = star;
        star->name = ""; // strtok(NULL, "|");
        token = strtok(NULL, "|");
        char *ra_str = strtok(NULL, "|");
        char *dec_str = strtok(NULL, "|");
        star->vis_mag = strtod(strtok(NULL, "|"), NULL);

        // Convert string-based HMS and DMS to radians
        struct ln_hms ra_hms = {0};
        ra_hms.hours = (unsigned short)strtol(strtok(ra_str, " "), NULL, 10);
        ra_hms.minutes = (unsigned short)strtol(strtok(NULL, " "), NULL, 10);
        ra_hms.seconds = strtod(strtok(NULL, " "), NULL);
        star->ra = 2 * PI - ln_hms_to_rad(&ra_hms);

        struct ln_dms dec_dms = {0};
        long degrees = strtol(strtok(dec_str, " "), NULL, 10);
        if (degrees < 0) {
            degrees *= -1;
            dec_dms.neg = 1;
        }
        dec_dms.degrees = (unsigned short)degrees;
        dec_dms.minutes = (unsigned short)strtol(strtok(NULL, " "), NULL, 10);
        dec_dms.seconds = strtod(strtok(NULL, " "), NULL);
        star->dec = ln_dms_to_rad(&dec_dms);

        // printf("Name: %s\n", stars[i].name);
        // printf("\tRA: %f\n", stars[i].ra);
        // printf("\tDec: %f\n", stars[i].dec);
        // printf("\tVis. Mag: %f\n", stars[i].vis_mag);
        free(line);
        line = NULL;
        star = NULL;
        i++;
    }
    // free(line);
    SetTargetFPS(144);

    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_FREE);
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);
        for (int i = 0; i < sizeof(render_table) / sizeof(render_data_t); i++) {
            if (render_table[i].render) {
                star_data_t star = *render_table[i].data;
                double x = STAR_DIST * cos(star.dec) * cos(star.ra);
                double z = STAR_DIST * cos(star.dec) * sin(star.ra);
                double y = STAR_DIST * sin(star.dec);
                Vector3 pos = {x, y, z};
                // double endX = (x - 0.001 * (x + 0.1));
                // double endY = (y - 0.001 * y);
                // double endZ = (z - 0.001 * z);
                // Vector3 endPos = {endX, endY, endZ};
                // DrawLine3D(pos, endPos, RAYWHITE);
                double radius = 1 / exp(star.vis_mag);
                DrawSphere(pos, radius < MIN_RADIUS ? MIN_RADIUS : radius, RAYWHITE);
            }
        }
        EndMode3D();
        EndDrawing();
    }
    fclose(star_file);
    fclose(ursa_maj);
    fclose(orion);
    for (int i = 0; i < sizeof(render_table) / sizeof(render_data_t); i++) {
      if (render_table[i].data != NULL) {
        free(render_table[i].data);
        render_table[i].data = NULL;
    }
    }
    CloseWindow();
    return 0;
}
