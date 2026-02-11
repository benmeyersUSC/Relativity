//
// Created by Ben Meyers on 2/11/26.
//
#include "Paddle.h"
#include "Game.h"

void Paddle::HandleInput(const bool *keyboardInput) {
    mVeloSign = keyboardInput[SDL_SCANCODE_RIGHT] - keyboardInput[SDL_SCANCODE_LEFT];
}

void Paddle::HandleUpdate(float deltaTime) {
    mVelos.push_back(mVelo);
    mVelo = Math::Clamp(mVelo + mVeloSign * PADDLE_ACCEL * deltaTime, -1000.0f, 1000.0f);

    mVelo *= Math::NearlyZero(PADDLE_ACCEL * mVeloSign) || PADDLE_ACCEL * mVeloSign * mVelo < 0.0f ? BRAKE_FACTOR
                                                                                     : 1.0f;

    GetTransform().PositionDelta(mVelo * deltaTime, 0.0f);

    if (GetTransform().GetPosition().x >= gGame.WINDOW_WIDTH) {
        mVelo = 0.0f;
        GetTransform().PositionDelta(gGame.WINDOW_WIDTH - GetTransform().GetPosition().x  , 0.0f);
    }
    else if (GetTransform().GetPosition().x <= 0.0f) {
        mVelo = 0.0f;
        GetTransform().PositionDelta(- GetTransform().GetPosition().x, 0.0f);
    }
}

Paddle::Paddle( unsigned frameLife) : Actor(frameLife) {
    GetTransform().SetSize(Vector2(PADDLE_WIDTH, PADDLE_HEIGHT)); // not quite bottom
}
