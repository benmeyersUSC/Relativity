//
// Created by Ben Meyers on 2/13/26.
//

#include "DrawComponent.h"
#include "Game.h"

DrawComponent::DrawComponent(class Actor *owner) : Component(owner) {
    mRenderer = gGame.GetRenderer();
}

void DrawComponent::HandleUpdate(float deltaTime) {
    Component::HandleUpdate(deltaTime);
}

void DrawComponent::HandleRender() {
    Component::HandleRender();
    for (auto& entry : mShapes) {
        std::visit([&](auto& e) { Draw(e); }, entry);
    }
    mShapes.clear();
}

void DrawComponent::DrawFloat(float x, float y, const char* fmt, float value, float scale) {
    mShapes.emplace_back(ShapeText{x, y, scale, FormatString(fmt, value), Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR});
}

void DrawComponent::DrawInt(float x, float y, int value, float scale) {
    mShapes.emplace_back(ShapeText{x, y, scale, FormatString("%i", value), Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR});
}


void DrawComponent::DrawFilledCircle(float cx, float cy, float radius, Uint8 r, Uint8 g,Uint8 b,Uint8 a) {
    float r2 = radius * radius;
    for (float dy = -radius; dy <= radius; dy += 1.0f) {
        float dx = Math::Sqrt(r2 - dy * dy);
        mShapes.emplace_back(ShapeRect{cx - dx, cy + dy, dx * 2.0f, 1.0f, r, g, b, a});
    }
}

void DrawComponent::SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const {
    SDL_SetRenderDrawColor(mRenderer, r, g, b, a);
}

void DrawComponent::Draw(const ShapeRect &rect) const {
    SetColor(rect.r, rect.g, rect.b, rect.a);
    SDL_FRect rct{rect.x, rect.y, rect.w, rect.h};
    SDL_RenderFillRect(mRenderer, &rct);
}

void DrawComponent::Draw(const ShapeText &text) const {
    SetColor( Game::MAX_COLOR,Game::MAX_COLOR,Game::MAX_COLOR,Game::MAX_COLOR);
    if (text.scale != 1.0f) { SDL_SetRenderScale(mRenderer, text.scale, text.scale); }
    SDL_RenderDebugTextFormat(mRenderer, text.x / text.scale, text.y / text.scale, "%s", text.text.c_str());
    if (text.scale != 1.0f) { SDL_SetRenderScale(mRenderer, 1.0f, 1.0f); }
}
