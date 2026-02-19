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
    mDraw->AddFilledCircle(pos.x, pos.y, radius, 0, 0, Game::MAX_COLOR, Game::MAX_COLOR);

    // now we type in the score onto the paddle
    Vector2 paddleRect = GetTransform().GetPosition();

    // this is so jank and boof...fix
    // pos + velo scaled 2x
    float posY = paddleRect.y - 10.0f / Game::HALF_CHAR_PIXELS - GetTransform().GetSize().y * 3 - 36.0f;
    float veloY = paddleRect.y - 10.0f / Game::HALF_CHAR_PIXELS - GetTransform().GetSize().y * 2 - 36.0f;
    float timeY = paddleRect.y - 10.0f / Game::HALF_CHAR_PIXELS - GetTransform().GetSize().y - 36.0f;
    mDraw->AddText(Game::HALF_WIDTH - Game::CHAR_PIXELS*27, posY, DrawComponent::FormatString("Space Position: %.0f px", GetTransform().GetPosition().x - Game::HALF_WIDTH), 2.0f);
    mDraw->AddText(Game::HALF_WIDTH - Game::CHAR_PIXELS*27, veloY, DrawComponent::FormatString("Space Velocity: %.0f px/s", GetSpacetime()->GetSpatialVelocity()), 2.0f);
    mDraw->AddText(Game::HALF_WIDTH - Game::CHAR_PIXELS*27, timeY, DrawComponent::FormatString("Aging at %.2f%% speed", GetSpacetime()->GetProperOverCoordinateTime() * 100.0f), 2.0f);

    // time remaining
    float timeBarWid = Game::PLOT_WIDTH;
    mDraw->AddScaledWidthRect(Game::HALF_WIDTH - timeBarWid/2.0f, Game::WINDOW_HEIGHT - 27.0f, timeBarWid, 27.0f, (1.0f - mSpacetime->ElapsedPct()),  135, Game::MAX_COLOR, 135, Game::MAX_COLOR,
        DrawComponent::FormatString("%.2fs remaining...", Game::DURATION_SECONDS - gGame.GetDT()), Game::CHAR_PIXELS, 2.7f);

}

void Player::HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons,
                         const Vector2& posMouse) {
    mVeloSign = keys[SDL_SCANCODE_RIGHT] - keys[SDL_SCANCODE_LEFT];
}

void Player::HandleUpdate(float deltaTime) {
    Actor::HandleUpdate(deltaTime);

    // increasing velocity becomes asymptotically hard as you approach max!
    // (and this obviates clamping!)
    float veloRatio = std::abs(mSpacetime->GetSpatialVelocity()) / Game::MAX_VELO;
    float realAcceleration = static_cast<float>(mVeloSign) * PLAYER_ACCEL *
        Math::Pow(1.0f - veloRatio, 2.0f);

    // integrate velocity by accel
    mSpacetime->GetSpatialVelocity() = mSpacetime->GetSpatialVelocity() + realAcceleration * deltaTime;
    // apply brake factor for no or (net)negative accel
    auto zeroAccel = Math::NearlyZero(realAcceleration);
    auto turningAround = realAcceleration * mSpacetime->GetSpatialVelocity() < 0.0f;
    mSpacetime->GetSpatialVelocity() *= turningAround ? BRAKE_FACTOR : 1.0f;

    // integrate position by velo
    GetTransform().PositionDelta(mSpacetime->GetSpatialVelocity() * deltaTime, 0.0f);

    FixPosition();
}

void Player::FixPosition() {
    // boundary collision
    float x = GetTransform().GetPosition().x;
    float halfW = GetTransform().GetSize().x / 2.0f;
    float minX = halfW;
    float maxX = Game::PLOT_WIDTH - halfW;

    if (x >= maxX || x <= minX) {
        mSpacetime->GetSpatialVelocity() = -mSpacetime->GetSpatialVelocity();
        float clampedX = std::clamp(x, minX, maxX);
        GetTransform().PositionDelta(clampedX - x, 0.0f);
    }
}

Player::Player() :Actor() {
    GetTransform().SetSize(Vector2(PLAYER_WIDTH, PLAYER_HEIGHT));
    mSpacetime = CreateComponent<SpacetimeComponent>();
    mDraw = CreateComponent<DrawComponent>();
}
