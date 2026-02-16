//
// Created by Ben Meyers on 2/15/26.
//

#include "SpacetimePlayer.h"
#include "ImageComponent.h"

#include "DrawComponent.h"
#include "Game.h"
#include "SpacetimeDisplay.h"

SpacetimePlayer::SpacetimePlayer() {
    mDraw = CreateComponent<DrawComponent>();
    mImage = CreateComponent<ImageComponent>();
    mImage->SetTexture(gGame.GetTexture(Game::ARROW_FILE));
    mImage->GetTransform().SetScale(0.1f);
}

void SpacetimePlayer::HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons, const Vector2 &posMouse) {
    Actor::HandleInput(keys, mouseButtons, posMouse);

    // player circle following pos
    // velo angle
    auto rotationSign = gGame.GetSpacetimeDisplay()->GetMouseBasedVelo() >= 0.0f ? 1.0f : -1.0f;
    mImage->GetTransform().SetRotation(rotationSign * Math::ToDegrees(Math::Acos(gGame.GetSpacetimeDisplay()->GetMouseBasedTime()/100.0f)));
    mImage->GetTransform().SetPosition({gGame.GetSpacetimeDisplay()->GetMouseBasedPos() + Game::HALF_WIDTH, gGame.GetMousePos().y });
}

void SpacetimePlayer::HandleRender() {
    Actor::HandleRender();
    float playerRadius = 27.0f;
    mDraw->AddFilledCircle(
    gGame.GetSpacetimeDisplay()->GetMouseBasedPos() + Game::HALF_WIDTH, gGame.GetMousePos().y, playerRadius,
    0, 0, Game::MAX_COLOR, Game::MAX_COLOR
    );

    mImage->Draw();
}
