#define CLAY_IMPLEMENTATION
#include <clay.h>
#include <CSFML/Graphics.h>
#include <CSFML/Window.h>
#include <CSFML/Config.h>
#include <CSFML/System.h>
#include <CSFML/OpenGL.h>
#include <math.h>

void Clay_SFML_Initialize(sfRenderWindow *window);
void Clay_SFML_Render(Clay_RenderCommandArray *commands);
void Clay_SFML_Close();

typedef struct {
    size_t cornerPointCount;
    sfVector2f size;
    float radius[4];
} shapedata_t;

size_t sfGetPointCountCallback(void *userData) {
    shapedata_t *data = (shapedata_t *)userData;
    return data->cornerPointCount * 4;
}
sfVector2f sfGetPointCallback(size_t index, void *userData) {
    shapedata_t *data = (shapedata_t *)userData;
    if (index >= data->cornerPointCount * 4)
        return (sfVector2f){0, 0};

    float deltaAngle = 90.0f / (data->cornerPointCount - 1);
    sfVector2f center;
    unsigned int centerIndex = index / data->cornerPointCount;
    static const float pi = 3.141592654f;
    switch (centerIndex) {
    case 0:
        center.x = data->size.x - data->radius[0];
        center.y = data->radius[0];
        break;
    case 1:
        center.x = data->radius[1];
        center.y = data->radius[1];
        break;
    case 2:
        center.x = data->radius[2];
        center.y = data->size.y - data->radius[2];
        break;
    case 3:
        center.x = data->size.x - data->radius[3];
        center.y = data->size.y - data->radius[3];
        break;
    }

    return (sfVector2f){
        data->radius[centerIndex] *
                cos(deltaAngle * (index - centerIndex) * pi / 180) +
            center.x,
        -data->radius[centerIndex] *
                sin(deltaAngle * (index - centerIndex) * pi / 180) +
            center.y};
}

sfRenderWindow *mainWindow;

void Clay_SFML_Initialize(sfRenderWindow *window) {
  mainWindow = window;
}

void Clay_SFML_Close() {

  sfRenderWindow_destroy(mainWindow);
}

void Clay_SFML_Render(Clay_RenderCommandArray *commands) {
}
