//
// Created by Ben Meyers on 2/11/26.
//
#include "Player.h"
#include "Game.h"
#include "SpacetimeComponent.h"
#include "DrawComponent.h"
#include <algorithm>

void Player::HandleRender() {
    const Vector2 pos = GetTransform().GetPosition();
    const float radius = GetTransform().GetSize().x / 2.0f;
    mDraw->AddFilledCircle(pos.x, pos.y, radius, 0, 0, Game::MAX_COLOR, Game::MAX_COLOR);

    const float lineH  = GetTransform().GetSize().y;
    constexpr float textX  = Game::HALF_WIDTH - Game::CHAR_PIXELS * 27;
    const float topY   = pos.y - lineH * 3 - 38.5f; // 38.5 = 2.5 pad + 36 offset

    mDraw->AddText(textX, topY,           DrawComponent::FormatString("Space Position: %.0f px",    pos.x - Game::HALF_WIDTH), 2.0f);
    mDraw->AddText(textX, topY + lineH,   DrawComponent::FormatString("Space Velocity: %.0f px/s",  GetSpacetime()->GetSpatialVelocity()), 2.0f);
    mDraw->AddText(textX, topY + lineH*2, DrawComponent::FormatString("Aging at %.2f%% speed",      GetSpacetime()->GetProperOverCoordinateTime() * 100.0f), 2.0f);

    constexpr float timeBarWid = Game::PLOT_WIDTH;
    mDraw->AddScaledWidthRect(
        Game::HALF_WIDTH - timeBarWid/2.0f, Game::WINDOW_HEIGHT - 27.0f,
        timeBarWid, 27.0f, 1.0f - gGame.ElapsedPct(),
        135, Game::MAX_COLOR, 135, Game::MAX_COLOR,
        DrawComponent::FormatString("%.2fs remaining...", Game::DURATION_SECONDS - gGame.GetDT()),
        Game::CHAR_PIXELS, 2.7f);
}

void Player::HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons,const Vector2& posMouse) {
    mVeloSign = keys[SDL_SCANCODE_RIGHT] - keys[SDL_SCANCODE_LEFT];
}

void Player::HandleUpdate(const float deltaTime) {
    Actor::HandleUpdate(deltaTime);

    // increasing velocity becomes asymptotically hard as you approach max!
    // (and this obviates clamping!)
    const float pctOfMaxVelo = Math::Abs(mSpacetime->GetSpatialVelocity()) / Game::MAX_VELO;
    const float realAcceleration = static_cast<float>(mVeloSign) * PLAYER_ACCEL * Math::Pow(1.0f - pctOfMaxVelo, 2.0f);
    const auto turningAround = realAcceleration * mSpacetime->GetSpatialVelocity() < 0.0f;

    // integrate velocity by accel
    mSpacetime->GetSpatialVelocity() = mSpacetime->GetSpatialVelocity() + realAcceleration * deltaTime;

    // apply brake factor for negative accel
    mSpacetime->GetSpatialVelocity() *= turningAround ? BRAKE_FACTOR : 1.0f;

    // integrate position by velo
    GetTransform().PositionDelta(mSpacetime->GetSpatialVelocity() * deltaTime, 0.0f);

    FixPosition();
}

void Player::FixPosition() {
    // boundary collision
    const float x = GetTransform().GetPosition().x;
    const float halfW = GetTransform().GetSize().x / 2.0f;
    const float minX = halfW;

    if (const float maxX = Game::PLOT_WIDTH - halfW; x >= maxX || x <= minX) {
        mSpacetime->GetSpatialVelocity() = -mSpacetime->GetSpatialVelocity();
        GetTransform().PositionDelta(Math::Clamp(x, minX, maxX) - x, 0.0f);
    }
}

Player::Player() :Actor() {
    GetTransform().SetSize(Vector2(PLAYER_WIDTH, PLAYER_HEIGHT));
    mSpacetime = CreateComponent<SpacetimeComponent>();
    mDraw = CreateComponent<DrawComponent>();
}
