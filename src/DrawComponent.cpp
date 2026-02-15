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

void DrawComponent::AddText(float x, float y, std::string_view txt, float scale) {
    mShapes.emplace_back(ShapeText{x, y, scale, txt.data(), Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR});
}


void DrawComponent::AddFilledCircle(float cx, float cy, float radius, Uint8 r, Uint8 g,Uint8 b,Uint8 a) {
    float r2 = radius * radius;
    for (float dy = -radius; dy <= radius; dy += 1.0f) {
        float dx = Math::Sqrt(r2 - dy * dy);
        mShapes.emplace_back(ShapeRect{cx - dx, cy + dy, dx * 2.0f, 1.0f, r, g, b, a});
    }
}

void DrawComponent::AddRect(float x, float y, float w, float h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    mShapes.emplace_back(ShapeRect{
        x, y , w, h, r, g, b, a
    });
}

void DrawComponent::AddScaledWidthRect(float x, float y, float maxW, float h, float pct, Uint8 r, Uint8 g,
    Uint8 b, Uint8 a, std::string_view endMarker, float pad, float textScale) {
    AddRect(x, y, maxW * pct, h, r, g, b, a);
    AddText(x + maxW * pct + pad * textScale, y - Game::HALF_CHAR_PIXELS * textScale + h/2.0f, endMarker, textScale);
}

void DrawComponent::AddScaledHeightRect(float x, float y, float w, float maxH, float pct, Uint8 r, Uint8 g,
    Uint8 b, Uint8 a, std::string_view endMarker, float pad, float textScale) {
    float realY = y + maxH * (1.0f - pct);
    AddRect(x, realY, w, maxH * pct, r, g, b, a);
    AddText(x - Game::HALF_CHAR_PIXELS * textScale + w/2.0f, realY - pad * textScale - Game::HALF_CHAR_PIXELS, endMarker, textScale);
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
