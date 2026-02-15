//
// Created by Ben Meyers on 2/15/26.
//

#ifndef RELATIVITY_LINE_H
#define RELATIVITY_LINE_H

#include "Actor.h"
#include "Game.h"
class DrawComponent;

class Line : public  Actor{
    static constexpr float POINT_SIZE = 1.0f;
    static constexpr float HALF_POINT_SIZE = POINT_SIZE / 2.0f;
    Vector2 mSlope;
    Vector2 mOrigin;
    DrawComponent* mDraw = nullptr;
public:
    static constexpr Vector2 X_AXIS_SLOPE(1.0f, 0.0f);
    static constexpr Vector2 X_AXIS_ORIGIN(0.0f, Game::HALF_HEIGHT);
    static constexpr Vector2 Y_AXIS_SLOPE(0.0f, 1.0f);
    static constexpr Vector2 Y_AXIS_ORIGIN(Game::HALF_WIDTH, 0.0f);

    Line();
    void SetSlope(const Vector2& slope) {mSlope = slope;}
    void SetOrigin (const Vector2& og) {mOrigin = og;}
protected:
    void HandleRender() override;
};


#endif //RELATIVITY_LINE_H