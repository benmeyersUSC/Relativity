//
// Created by Ben Meyers on 2/11/26.
//
#include "Player.h"
#include "Game.h"
#include "SpacetimeComponent.h"
#include <algorithm>
void Player::HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons,
                             const Vector2& posMouse) {
    mVeloSign = keys[SDL_SCANCODE_RIGHT] - keys[SDL_SCANCODE_LEFT];
}

void Player::HandleUpdate(float deltaTime) {
    Actor::HandleUpdate(deltaTime);

    // increasing velocity becomes asymptotically hard as you approach max!
    // (and this obviates clamping!)
    float realAcceleration = static_cast<float>(mVeloSign) * PLAYER_ACCEL * (
        1.0f -          std::abs(mSpacetimeComponent->GetVelocity()) /
                                        Game::MAX_VELO
        );

    // integrate velocity by accel
    mSpacetimeComponent->GetVelocity() = mSpacetimeComponent->GetVelocity() + realAcceleration * deltaTime;
    // apply brake factor for no or (net)negative accel
    auto zeroAccel = Math::NearlyZero(realAcceleration);
    auto turningAround = realAcceleration * mSpacetimeComponent->GetVelocity() < 0.0f;
    mSpacetimeComponent->GetVelocity() *= zeroAccel || turningAround ? BRAKE_FACTOR : 1.0f;

    // integrate position by velo
    GetTransform().PositionDelta(mSpacetimeComponent->GetVelocity() * deltaTime, 0.0f);

    FixPosition();
}

void Player::FixPosition() {
    // boundary collision
    float x = GetTransform().GetPosition().x;
    float halfW = GetTransform().GetSize().x / 2.0f;
    float minX = halfW;
    float maxX = Game::WINDOW_WIDTH - halfW;

    if (x >= maxX || x <= minX) {
        mSpacetimeComponent->GetVelocity() = 0.0f;
        float clampedX = std::clamp(x, minX, maxX);
        GetTransform().PositionDelta(clampedX - x, 0.0f);
    }
}

Player::Player() :Actor() {
    GetTransform().SetSize(Vector2(PLAYER_WIDTH, PLAYER_HEIGHT));
    mSpacetimeComponent = CreateComponent<SpacetimeComponent>();
}
