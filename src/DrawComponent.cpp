//
// Created by Ben Meyers on 2/13/26.
//

#include "DrawComponent.h"
#include "Game.h"

DrawComponent::DrawComponent(class Actor *owner) : Component(owner) {
    mRenderer = gGame.GetRenderer();
    gGame.AddRenderable(this);
}

DrawComponent::~DrawComponent() {
    gGame.RemoveRenderable(this);
}

void DrawComponent::HandleUpdate(float deltaTime) {
    Component::HandleUpdate(deltaTime);
}

void DrawComponent::HandleRender() {
    Component::HandleRender();
    // deal with blend mode just once
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    for (const auto& shape : mShapes) {
        shape->Draw(mRenderer);
    }
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_NONE);
    mShapes.clear();
}

void DrawComponent::AddText(float x, float y, std::string_view txt, float scale) {
    mShapes.push_back(std::make_unique<Text>(x, y, scale, std::string(txt), Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR));
}

void DrawComponent::AddFilledCircle(float cx, float cy, float radius, Uint8 r, Uint8 g,Uint8 b,Uint8 a) {
    mShapes.push_back(std::make_unique<Circle>(cx, cy, radius, r, g, b, a));
}

void DrawComponent::AddLine(float x1, float y1, float x2, float y2, Uint8 r, Uint8 g,Uint8 b,Uint8 a) {
    mShapes.push_back(std::make_unique<LineSegment>(x1, y1, x2, y2, r, g, b, a));
}

void DrawComponent::AddRect(float x, float y, float w, float h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    mShapes.push_back(std::make_unique<Rect>(x, y, w, h, r, g, b, a));
}

void DrawComponent::AddOutlineRect(float x, float y, float w, float h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    mShapes.push_back(std::make_unique<Rect>(x, y, w, h, r, g, b, a, false));
}

void DrawComponent::AddScaledWidthRect(float x, float y, float maxW, float h, float pct, Uint8 r, Uint8 g,
    Uint8 b, Uint8 a, std::string_view endMarker, float pad, float textScale, bool reversed) {
    const float len = maxW * pct;
    const float textCenterY = y - Game::HALF_CHAR_PIXELS * textScale + h / 2.0f;
    if (reversed) {
        // bar grows left from x, text on left end
        AddRect(x - len, y, len, h, r, g, b, a);
        // if we're growing out left, we need to account for where to start the text
        const float markerW = Game::CHAR_PIXELS * textScale * static_cast<float>(endMarker.size());
        AddText(x - len - pad * textScale - markerW, textCenterY, endMarker, textScale);
    } else {
        // bar grows right from x, text on right end
        AddRect(x, y, len, h, r, g, b, a);
        AddText(x + len + pad * textScale, textCenterY, endMarker, textScale);
    }
}

void DrawComponent::AddScaledHeightRect(float x, float y, float w, float maxH, float pct, Uint8 r, Uint8 g, Uint8 b, Uint8 a, std::string_view endMarker, float pad, float textScale, bool reversed) {
    // true length to render
    const float len = maxH * pct;
    // center of bar - half the text width
    const float textStartX = x + w / 2.0f - Game::HALF_CHAR_PIXELS * textScale * static_cast<float>(endMarker.size());
    if (reversed) {
        // bar grows down from y, text below bottom of bar
        AddRect(x, y, w, len, r, g, b, a);
        AddText(textStartX, y + len + pad * textScale, endMarker, textScale);
    } else {
        // bar grows up from bottom (y + maxH), text above top of bar
        const float realY = y + maxH - len;
        AddRect(x, realY, w, len, r, g, b, a);
        // text's y needs to shift up by pad * text scale
        AddText(textStartX, realY - textScale * (pad - Game::CHAR_PIXELS), endMarker, textScale);
    }
}


void DrawComponent::Rect::Draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, _r, _g, _b, _a);
    SDL_FRect rct{_x, _y, _w, _h};
    if (_filled) {
        SDL_RenderFillRect(renderer, &rct);
    }
    else {
        SDL_RenderRect(renderer, &rct);
    }
}

void DrawComponent::Text::Draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, _r, _g, _b, _a);
    SDL_SetRenderScale(renderer, _scale, _scale);
    SDL_RenderDebugTextFormat(renderer, _x / _scale, _y / _scale, "%s", _text.c_str());
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);
}

void DrawComponent::LineSegment::Draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, _r, _g, _b, _a);
    SDL_RenderLine(renderer, _x, _y, _x2, _y2);
}

void DrawComponent::Circle::Draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, _r, _g, _b, _a);
    // x^2 + y^2 = r^2
    const float r2 = _radius * _radius;
    for (float dy = -_radius; dy <= _radius; dy += 0.5f) {
        // x = +/- sqrt(r^2 - y^2)
        // so if we start it at -sqrt(r^2 - y^2) and make width 2*dx, we reach + sqrt(r^2 - y^2)
        const float dx = Math::Sqrt(r2 - dy * dy);
        SDL_FRect rct{_x - dx, _y + dy, dx * 2.0f, 0.5f};
        SDL_RenderFillRect(renderer, &rct);
    }
}
