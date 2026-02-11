//
// Created by Ben Meyers on 2/11/26.
//
#include "Paddle.h"
#include "Game.h"

void Paddle::HandleInput(const bool *keyboardInput) {
    mVeloSign += keyboardInput[SDL_SCANCODE_RIGHT] - keyboardInput[SDL_SCANCODE_LEFT];
}

void Paddle::HandleUpdate(float deltaTime) {
    mVelos.push_back(mVelo);
    mVelo += mVeloSign * PADDLE_ACCEL * deltaTime;
    GetTransform().PositionDelta(mVelo * deltaTime, 1.0f);
}

Paddle::Paddle( unsigned frameLife) : Actor(frameLife) {
    GetTransform().SetSize(Vector2(PADDLE_WIDTH, PADDLE_HEIGHT)); // not quite bottom
}
