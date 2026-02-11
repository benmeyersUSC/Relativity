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
    const Vector2& GetPosition() const { return mPosition; }
    const Vector2& GetSize() const { return mSize; }

    void SetPosition(const Vector2& pos) { mPosition = pos; }
    void SetSize(const Vector2& sz) { mSize = sz; }

    SDL_FRect GetRect() const;

private:
    // x, y
    Vector2 mPosition;
    // width, height
    Vector2 mSize;
};


#endif //RELATIVITY_TRANSFORM_H