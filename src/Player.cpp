//
// Created by Ben Meyers on 2/11/26.
//
#include "Player.h"
#include "Game.h"

void Player::HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons,
                             const Vector2& posMouse) {
    mVeloSign = keys[SDL_SCANCODE_RIGHT] - keys[SDL_SCANCODE_LEFT];
}

void Player::HandleUpdate(float deltaTime) {
    Actor::HandleUpdate(deltaTime);

    // increasing velocity becomes asymptotically hard as you approach max!
    float realAcceleration = PLAYER_ACCEL * (1.0f - std::abs(mVelo) / Game::MAX_VELO);

    mVelo = Math::Clamp(mVelo + static_cast<float>(mVeloSign) * realAcceleration * deltaTime, -Game::MAX_VELO, Game::MAX_VELO);
    mVelo *= Math::NearlyZero(realAcceleration * static_cast<float>(mVeloSign)) || realAcceleration * static_cast<float>(mVeloSign) * mVelo < 0.0f ? BRAKE_FACTOR
                                                                                     : 1.0f;

    GetTransform().PositionDelta(mVelo * deltaTime, 0.0f);


    // booooof
    if (GetTransform().GetPosition().x + GetTransform().GetSize().x/2.0f >= Game::WINDOW_WIDTH) {
        mVelo = 0.0f;
        GetTransform().PositionDelta(Game::WINDOW_WIDTH - GetTransform().GetPosition().x - GetTransform().GetSize().x/2.0f  , 0.0f);
    }
    else if (GetTransform().GetPosition().x - GetTransform().GetSize().x/2.0f <= 0.0f) {
        mVelo = 0.0f;
        GetTransform().PositionDelta(GetTransform().GetSize().x/2.0f - GetTransform().GetPosition().x, 0.0f);
    }
}

Player::Player( unsigned frameLife) : Actor(frameLife) {
    GetTransform().SetSize(Vector2(PLAYER_WIDTH, PLAYER_HEIGHT));
}
