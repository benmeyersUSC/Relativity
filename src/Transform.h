//
// Created by Ben Meyers on 2/11/26.
//

#ifndef RELATIVITY_TRANSFORM_H
#define RELATIVITY_TRANSFORM_H

#include "Math.h"
#include "SDL3/SDL_rect.h"
using Math::Vector2;
class Transform {
public:
    [[nodiscard]] const Vector2& GetPosition() const { return mPosition; }
    [[nodiscard]] const Vector2& GetSize() const { return mSize; }

    void SetPosition(const Vector2& pos) { mPosition = pos; }
    void PositionDelta(float x, float y){mPosition.x += x; mPosition.y += y;}
    void PositionScale(float x, float y){mPosition.x *= x; mPosition.y *= y;}
    void SetSize(const Vector2& sz) { mSize = sz; }

    [[nodiscard]] SDL_FRect GetRect() const;

private:
    // x, y
    Vector2 mPosition;
    // width, height
    Vector2 mSize;
};


#endif //RELATIVITY_TRANSFORM_H