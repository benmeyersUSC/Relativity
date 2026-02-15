//
// Created by Ben Meyers on 2/11/26.
//
#include "Player.h"
#include "Game.h"
#include "SpacetimeComponent.h"
#include "DrawComponent.h"
#include <algorithm>

void Player::HandleRender() {
    Vector2 pos = GetTransform().GetPosition();
    float radius = GetTransform().GetSize().x / 2.0f;
    mDraw->DrawFilledCircle(pos.x, pos.y, radius, 0, 0, Game::MAX_COLOR, Game::MAX_COLOR);

    Actor::HandleRender();
    // now we type in the score onto the paddle
    Vector2 paddleRect = GetTransform().GetPosition();

    // time remaining
    mDraw->DrawInt(paddleRect.x - Game::HALF_CHAR_PIXELS, paddleRect.y - 10.0f / Game::HALF_CHAR_PIXELS, static_cast<int>(Game::DURATION_SECONDS - gGame.GetDT()), 1.35f);

    // this is so jank and boof...fix
    // pos + velo scaled 2x
    float posY = paddleRect.y - 10.0f / Game::HALF_CHAR_PIXELS - GetTransform().GetSize().y * 3 - 36.0f;
    float veloY = paddleRect.y - 10.0f / Game::HALF_CHAR_PIXELS - GetTransform().GetSize().y * 2 - 36.0f;
    float timeY = paddleRect.y - 10.0f / Game::HALF_CHAR_PIXELS - GetTransform().GetSize().y - 36.0f;
    mDraw->DrawFloat(Game::HALF_WIDTH - Game::CHAR_PIXELS*27, posY, "Space Position: %.0f px", GetTransform().GetPosition().x - Game::HALF_WIDTH, 2.0f);
    mDraw->DrawFloat(Game::HALF_WIDTH - Game::CHAR_PIXELS*27, veloY, "Space Velocity: %.0f px/s", GetSpacetime()->GetVelocity(), 2.0f);
    mDraw->DrawFloat(Game::HALF_WIDTH - Game::CHAR_PIXELS*27, timeY, "Aging at %.2f%% speed", GetSpacetime()->GetTimeFactor() * 100.0f, 2.0f);
}

void Player::HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons,
                         const Vector2& posMouse) {
    mVeloSign = keys[SDL_SCANCODE_RIGHT] - keys[SDL_SCANCODE_LEFT];
}

void Player::HandleUpdate(float deltaTime) {
    Actor::HandleUpdate(deltaTime);

    // increasing velocity becomes asymptotically hard as you approach max!
    // (and this obviates clamping!)
    float realAcceleration = static_cast<float>(mVeloSign) * PLAYER_ACCEL * (
        1.0f -          std::abs(mSpacetime->GetVelocity()) /
                                        Game::MAX_VELO
        );

    // integrate velocity by accel
    mSpacetime->GetVelocity() = mSpacetime->GetVelocity() + realAcceleration * deltaTime;
    // apply brake factor for no or (net)negative accel
    auto zeroAccel = Math::NearlyZero(realAcceleration);
    auto turningAround = realAcceleration * mSpacetime->GetVelocity() < 0.0f;
    mSpacetime->GetVelocity() *= zeroAccel || turningAround ? BRAKE_FACTOR : 1.0f;

    // integrate position by velo
    GetTransform().PositionDelta(mSpacetime->GetVelocity() * deltaTime, 0.0f);

    FixPosition();
}

void Player::FixPosition() {
    // boundary collision
    float x = GetTransform().GetPosition().x;
    float halfW = GetTransform().GetSize().x / 2.0f;
    float minX = halfW;
    float maxX = Game::WINDOW_WIDTH - halfW;

    if (x >= maxX || x <= minX) {
        mSpacetime->GetVelocity() = 0.0f;
        float clampedX = std::clamp(x, minX, maxX);
        GetTransform().PositionDelta(clampedX - x, 0.0f);
    }
}

Player::Player() :Actor() {
    GetTransform().SetSize(Vector2(PLAYER_WIDTH, PLAYER_HEIGHT));
    mSpacetime = CreateComponent<SpacetimeComponent>();
    mDraw = CreateComponent<DrawComponent>();
}
