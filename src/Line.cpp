//
// Created by Ben Meyers on 2/15/26.
//

#include "Line.h"
#include "DrawComponent.h"
#include "Game.h"

Line::Line() : Actor() {
    mDraw = CreateComponent<DrawComponent>();
}

void Line::HandleRender() {
    bool steep = Math::Abs(mSlope.y) > Math::Abs(mSlope.x);

    if (steep) {
        // step along y, compute x
        float ratio = Math::NearlyZero(mSlope.y) ? 0.0f : mSlope.x / mSlope.y;
        for (float y = 0.0f; y < Game::WINDOW_HEIGHT; y += 1.0f) {
            float x = mOrigin.x + ratio * (y - mOrigin.y);
            mDraw->AddRect(x - HALF_POINT_SIZE, y - HALF_POINT_SIZE, POINT_SIZE, POINT_SIZE,
                           Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR);
        }
    } else {
        // step along x, compute y
        float ratio = Math::NearlyZero(mSlope.x) ? 0.0f : mSlope.y / mSlope.x;
        for (float x = 0.0f; x < Game::WINDOW_WIDTH; x += 1.0f) {
            float y = mOrigin.y + ratio * (x - mOrigin.x);
            mDraw->AddRect(x - HALF_POINT_SIZE, y - HALF_POINT_SIZE, POINT_SIZE, POINT_SIZE,
                           Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR, Game::MAX_COLOR);
        }
    }

    Actor::HandleRender();
}
