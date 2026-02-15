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


class DrawComponent : public Component{

protected:
    explicit DrawComponent(class Actor *owner);
    void HandleUpdate(float deltaTime) override;
    void HandleRender() override;

    friend class Actor;
public:
    template <typename T>
    static std::string FormatString(const char* fmt, T val);
    void AddText(float x, float y, std::string_view txt, float scale = 1.0f);
    void AddFilledCircle(float cx, float cy, float radius, Uint8 r, Uint8 g,Uint8 b,Uint8 a);
    void AddRect(float x, float y, float w, float h, Uint8 r, Uint8 g,Uint8 b,Uint8 a);
    void AddScaledWidthRect(float x, float y, float maxW, float h, float pct, Uint8 r, Uint8 g,Uint8 b,Uint8 a, std::string_view endMarker = "", float pad = 0.0f, float textScale = 1.0f);
    void AddScaledHeightRect(float x, float y, float w, float maxH, float pct, Uint8 r, Uint8 g,Uint8 b,Uint8 a, std::string_view endMarker = "", float pad = 0.0f, float textScale = 1.0f);

    void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const;

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


    void Draw(const ShapeRect& rect) const;
    void Draw(const ShapeText& text) const;

private:
    SDL_Renderer* mRenderer = nullptr;
    std::vector<ShapeEntry> mShapes;
};

template<typename T>
std::string DrawComponent::FormatString(const char *fmt, T val) {
    char tempStr[128];
    std::snprintf(tempStr, sizeof(tempStr), fmt, val);
    return {tempStr};
}


#endif //RELATIVITY_SHAPECOMPONENT_H
