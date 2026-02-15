//
// Created by Ben Meyers on 2/13/26.
//

#include "ImageComponent.h"
#include "SDL3/SDL.h"
#include "Actor.h"
#include "Game.h"

void ImageComponent::Draw()
{
    if (mTexture == nullptr)
    {
        return;
    }
    SDL_FRect rect = SDL_FRect();
    SDL_GetTextureSize(mTexture, &rect.w, &rect.h);
    float scale = GetTransform().GetScale();
    rect.w *= scale;
    rect.h *= scale;

    rect.x = GetTransform().GetPosition().x - rect.w / 2.0f;
    rect.y = GetTransform().GetPosition().y - rect.h / 2.0f;

    SDL_RenderTextureRotated(mRenderer, mTexture, nullptr, &rect, GetTransform().GetRotation(), nullptr,
                             SDL_FLIP_NONE);
}

void ImageComponent::DrawWithPivot(SDL_FPoint pivot)
{
    if (mTexture == nullptr)
    {
        return;
    }
    SDL_FRect rect = SDL_FRect();
    SDL_GetTextureSize(mTexture, &rect.w, &rect.h);
    float scale = GetTransform().GetScale();
    rect.w *= scale;
    rect.h *= scale;

    // position so the pivot point in the texture lands at mTransform position
    rect.x = GetTransform().GetPosition().x - pivot.x;
    rect.y = GetTransform().GetPosition().y - pivot.y;

    SDL_RenderTextureRotated(mRenderer, mTexture, nullptr, &rect, GetTransform().GetRotation(), &pivot,
                             SDL_FLIP_NONE);
}

void ImageComponent::SetTexture(SDL_Texture* texture)
{
    mTexture = texture;
}

Transform & ImageComponent::GetTransform() {
    return mUseLocalTransform ? mLocalTransform : GetOwner()->GetTransform();
}

ImageComponent::ImageComponent(class Actor *owner) : Component(owner) {
    mRenderer = gGame.GetRenderer();
}

void ImageComponent::HandleRender() {
    Component::HandleRender();
    if (mHasPivot) {
        DrawWithPivot(mPivot);
    } else {
        Draw();
    }
}

