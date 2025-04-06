#define CLAY_IMPLEMENTATION
#include <clay.h>
#include <CSFML/Graphics.h>
#include <CSFML/Window.h>
#include <CSFML/Config.h>
#include <CSFML/System.h>
#include <CSFML/OpenGL.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#define CLAY_COLOR_TO_SFCOLOR(color)                                           \
    (sfColor) { color.r, color.b, color.g, color.a }
#define CLAY_BOUNDS_POS(bounds)                                                \
    (sfVector2f) { bounds.x, bounds.y }
#define CLAY_BOUNDS_DIM(bounds)                                                \
    (sfVector2f) { bounds.width, bounds.height }

void Clay_SFML_Initialize(sfRenderWindow *window);
void Clay_SFML_Render(Clay_RenderCommandArray commands, sfFont **fonts);
void Clay_SFML_Close();
Clay_Dimensions Clay_SFML_MeasureText(Clay_StringSlice string,
                                      Clay_TextElementConfig *config,
                                      void *userData);

sfRenderWindow *mainWindow;

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

void Clay_SFML_Initialize(sfRenderWindow *window) { mainWindow = window; }

void Clay_SFML_Close() { sfRenderWindow_destroy(mainWindow); }

// Mostly copied from the example Raylib renderer but obviously modified to work
// with SFML
Clay_Dimensions Clay_SFML_MeasureText(Clay_StringSlice string,
                                      Clay_TextElementConfig *config,
                                      void *userData) {
    Clay_Dimensions textSize = {0};
    float maxTextWidth = 0.0f;
    float lineTextWidth = 0;

    float textHeight = config->fontSize;
    sfFont **fonts = (sfFont **)userData;
    sfFont *fontToUse = fonts[config->fontId];

    for (int i = 0; i < string.length; ++i) {
        if (string.chars[i] == '\n') {
            maxTextWidth = fmax(maxTextWidth, lineTextWidth);
            lineTextWidth = 0;
            continue;
        }
        sfGlyph glyph =
            sfFont_getGlyph(fontToUse, string.chars[i], textHeight, false, 0);
        if (glyph.advance != 0)
            lineTextWidth += glyph.advance;
        else
            lineTextWidth +=
                (glyph.textureRect.size.x + glyph.textureRect.position.x);
    }

    maxTextWidth = fmax(maxTextWidth, lineTextWidth);

    textSize.width = maxTextWidth;
    textSize.height = textHeight;
    return textSize;
}

void Clay_SFML_Render(Clay_RenderCommandArray commands, sfFont **fonts) {
    sfRenderWindow_clear(mainWindow, (sfColor){0, 0, 0, 255});
    for (int index = 0; index < commands.length; index++) {
        Clay_RenderCommand *command =
            Clay_RenderCommandArray_Get(&commands, index);
        switch (command->commandType) {
        case CLAY_RENDER_COMMAND_TYPE_NONE:
            break;
        case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
            Clay_RectangleRenderData *config = &command->renderData.rectangle;
            Clay_CornerRadius corners = config->cornerRadius;
            shapedata_t userData = {
                .cornerPointCount = MAX_CORNER_POINTS,
                .size = {command->boundingBox.width,
                         command->boundingBox.height},
                .radius = {corners.topRight, corners.topLeft,
                           corners.bottomLeft, corners.bottomRight}};
            sfShape *rect = sfShape_create(sfGetPointCountCallback,
                                           sfGetPointCallback, &userData);
            sfShape_update(rect);
            sfColor background = sfColor_fromRGBA(
                config->backgroundColor.r, config->backgroundColor.g,
                config->backgroundColor.b, config->backgroundColor.a);
            sfShape_setFillColor(rect, background);
            sfShape_setPosition(rect, (sfVector2f){command->boundingBox.x,
                                                   command->boundingBox.y});
            sfRenderWindow_drawShape(mainWindow, rect, &sfRenderStates_default);
            sfShape_destroy(rect);
        }
        case CLAY_RENDER_COMMAND_TYPE_BORDER: {
            Clay_RectangleRenderData *config = &command->renderData.rectangle;
            Clay_CornerRadius corners = config->cornerRadius;
            // Just pick a border radius since they should always be the same
            size_t border_width = command->renderData.border.width.left;
            shapedata_t userData = {
                .cornerPointCount = MAX_CORNER_POINTS,
                .size = {command->boundingBox.width - border_width,
                         command->boundingBox.height - border_width},
                .radius = {corners.topRight - border_width / 2.0,
                           corners.topLeft - border_width / 2.0,
                           corners.bottomLeft - border_width / 2.0,
                           corners.bottomRight - border_width / 2.0}};
            sfShape *rect = sfShape_create(sfGetPointCountCallback,
                                           sfGetPointCallback, &userData);
            sfShape_update(rect);
            sfColor outline = sfColor_fromRGBA(
                config->backgroundColor.r, config->backgroundColor.g,
                config->backgroundColor.b, config->backgroundColor.a);
            sfShape_setOutlineColor(rect, outline);
            sfShape_setOutlineThickness(rect, border_width);
            sfShape_setFillColor(rect, sfTransparent);
            sfShape_setPosition(
                rect,
                (sfVector2f){command->boundingBox.x + border_width / 2.0,
                             command->boundingBox.y + border_width / 2.0});
            sfRenderWindow_drawShape(mainWindow, rect, &sfRenderStates_default);
            sfShape_destroy(rect);
        } break;
        case CLAY_RENDER_COMMAND_TYPE_TEXT: {
            Clay_TextRenderData *textData = &command->renderData.text;
            sfFont *font = fonts[textData->fontId];
            sfText *text = sfText_create(font);

            int32_t length = textData->stringContents.length;
            // Create a null-terminated string if the slice isn't
            // null-terminated
            char *string;
            bool malloced = false;
            if (textData->stringContents.chars[length - 1] != '\0') {
                string = malloc(length + sizeof(char));
                memcpy(string, (char *)textData->stringContents.chars, length);
                string[length] = '\0';
                malloced = true;
            } else
                string = (char *)textData->stringContents.chars;

            sfText_setString(text, string);

            sfText_setCharacterSize(text, textData->fontSize);
            sfText_setFillColor(text,
                                CLAY_COLOR_TO_SFCOLOR(textData->textColor));
            // sfText_setLetterSpacing(text, textData->letterSpacing);
            sfText_setPosition(text, CLAY_BOUNDS_POS(command->boundingBox));
            sfRenderWindow_drawText(mainWindow, text, &sfRenderStates_default);
            sfText_destroy(text);
            if (malloced)
              free(string);
        } break;
        case CLAY_RENDER_COMMAND_TYPE_IMAGE:
            break;
        case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START:
            break;
        case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END:
            break;
        case CLAY_RENDER_COMMAND_TYPE_CUSTOM:
            break;
        }
    }
    sfRenderWindow_display(mainWindow);
}
