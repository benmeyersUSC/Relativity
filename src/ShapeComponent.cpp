//
// Created by Ben Meyers on 2/13/26.
//

#include "ShapeComponent.h"

#include <cstdio>

#include "Game.h"

std::string ShapeComponent::FormatString(const char *fmt, float val) {
    char tempStr[128];
    std::snprintf(tempStr, sizeof(tempStr), fmt, val);
    return {tempStr};
}
std::string ShapeComponent::FormatString(const char *fmt, int val) {
    char tempStr[128];
    std::snprintf(tempStr, sizeof(tempStr), fmt, val);
    return {tempStr};
}

ShapeComponent::ShapeComponent(class Actor *owner) : Component(owner) {
    mRenderer = gGame.GetRenderer();
}

void ShapeComponent::HandleUpdate(float deltaTime) {
    Component::HandleUpdate(deltaTime);
}

void ShapeComponent::HandleRender() {
    Component::HandleRender();
    for (auto& entry : mShapes) {
        std::visit([&](auto& e) { Draw(e); }, entry);
    }
    mShapes.clear();
}

void ShapeComponent::DrawFloat(float x, float y, const char* fmt, float value, float scale) {
    // SDL_SetRenderDrawColor(mRenderer, Game::MAX_COLOR,Game::MAX_COLOR,Game::MAX_COLOR,Game::MAX_COLOR);
    // if (scale != 1.0f) { SDL_SetRenderScale(mRenderer, scale, scale); }
    // SDL_RenderDebugTextFormat(mRenderer, x / scale, y / scale, fmt, value);
    // if (scale != 1.0f) { SDL_SetRenderScale(mRenderer, 1.0f, 1.0f); }

    // mShapes.emplace_back({x, y, scale, std::format(fmt, value),Game::MAX_COLOR,Game::MAX_COLOR,Game::MAX_COLOR,Game::MAX_COLOR });

    mShapes.emplace_back(ShapeText{x, y, scale, FormatString(fmt, value), Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR});
}

void ShapeComponent::DrawInt(float x, float y, int value, float scale) {
    // SDL_SetRenderDrawColor(mRenderer, Game::MAX_COLOR,Game::MAX_COLOR,Game::MAX_COLOR,Game::MAX_COLOR);
    // if (scale != 1.0f) { SDL_SetRenderScale(mRenderer, scale, scale); }
    // SDL_RenderDebugTextFormat(mRenderer, x / scale, y / scale, "%i", value);
    // if (scale != 1.0f) { SDL_SetRenderScale(mRenderer, 1.0f, 1.0f); }
    mShapes.emplace_back(ShapeText{x, y, scale, FormatString("%i", value), Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR});

}

void ShapeComponent::DrawFilledCircle(float cx, float cy, float radius, Uint8 r, Uint8 g,Uint8 b,Uint8 a) {
    // scan-line filled circle: uses current draw color
    float r2 = radius * radius;
    for (float dy = -radius; dy <= radius; dy += 1.0f) {
        float dx = Math::Sqrt(r2 - dy * dy);
        // SDL_FRect line(cx - dx, cy + dy, dx * 2.0f, 1.0f);
        // SDL_RenderFillRect(mRenderer, &line);
        mShapes.emplace_back(ShapeRect{cx - dx, cy + dy, dx * 2.0f, 1.0f, r, g, b, a});
    }
}

void ShapeComponent::SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(mRenderer, r, g, b, a);
}

void ShapeComponent::Draw(const ShapeRect &rect) {
    SDL_FRect rct{rect.x, rect.y, rect.w, rect.h};
    SDL_RenderFillRect(mRenderer, &rct);
}

void ShapeComponent::Draw(const ShapeText &text) {
    SetColor( Game::MAX_COLOR,Game::MAX_COLOR,Game::MAX_COLOR,Game::MAX_COLOR);
    if (text.scale != 1.0f) { SDL_SetRenderScale(mRenderer, text.scale, text.scale); }
    SDL_RenderDebugTextFormat(mRenderer, text.x / text.scale, text.y / text.scale, "%s", text.text.c_str());
    if (text.scale != 1.0f) { SDL_SetRenderScale(mRenderer, 1.0f, 1.0f); }
}
