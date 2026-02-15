//
// Created by Ben Meyers on 2/13/26.
//

#ifndef RELATIVITY_SHAPECOMPONENT_H
#define RELATIVITY_SHAPECOMPONENT_H
#include <string>
#include <variant>
#include <vector>

#include "Component.h"
#include "SDL3/SDL_render.h"


class ShapeComponent : public Component{

    static std::string FormatString(const char* fmt, float val);
    static std::string FormatString(const char* fmt, int val);
protected:
    explicit ShapeComponent(class Actor *owner);
    void HandleUpdate(float deltaTime) override;
    void HandleRender() override;

    friend class Actor;
public:
    void DrawFloat(float x, float y, const char* fmt, float value, float scale = 1.0f);
    void DrawInt(float x, float y, int value, float scale = 1.0f);
    void DrawFilledCircle(float cx, float cy, float radius, Uint8 r, Uint8 g,Uint8 b,Uint8 a);
    void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    struct ShapeRect {
        float x, y, w, h;
        Uint8 r, g, b, a;
    };

    struct ShapeText {
        float x, y, scale;
        std::string text;
        Uint8 r, g, b, a;
    };
    using ShapeEntry = std::variant<ShapeRect, ShapeText>;


    void Draw(const ShapeRect& rect);
    void Draw(const ShapeText& text);

private:
    SDL_Renderer* mRenderer = nullptr;
    std::vector<ShapeEntry> mShapes;
};


#endif //RELATIVITY_SHAPECOMPONENT_H