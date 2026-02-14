//
// Created by Ben Meyers on 2/13/26.
//

#include "ImageComponent.h"
void ImageComponent::Draw(SDL_Renderer* renderer)
{
    if (mTexture == nullptr)
    {
        return;
    }
    SDL_FRect rect = SDL_FRect();
    SDL_GetTextureSize(mTexture, &rect.w, &rect.h);
    float scale = mTransform.GetScale();
    rect.w *= scale;
    rect.h *= scale;

    rect.x = mTransform.GetPosition().x - rect.w / 2.0f;
    rect.y = mTransform.GetPosition().y - rect.h / 2.0f;

    SDL_RenderTextureRotated(renderer, mTexture, nullptr, &rect, mTransform.GetRotation(), nullptr,
                             SDL_FLIP_NONE);
}

void ImageComponent::DrawWithPivot(SDL_Renderer* renderer, SDL_FPoint pivot)
{
    if (mTexture == nullptr)
    {
        return;
    }
    SDL_FRect rect = SDL_FRect();
    SDL_GetTextureSize(mTexture, &rect.w, &rect.h);
    float scale = mTransform.GetScale();
    rect.w *= scale;
    rect.h *= scale;

    // position so the pivot point in the texture lands at mTransform position
    rect.x = mTransform.GetPosition().x - pivot.x;
    rect.y = mTransform.GetPosition().y - pivot.y;

    SDL_RenderTextureRotated(renderer, mTexture, nullptr, &rect, mTransform.GetRotation(), &pivot,
                             SDL_FLIP_NONE);
}

void ImageComponent::SetTexture(SDL_Texture* texture)
{
    mTexture = texture;
}

ImageComponent::ImageComponent(class Actor *owner) : Component(owner) {
}

void ImageComponent::HandleUpdate(float deltaTime) {
    Component::HandleUpdate(deltaTime);
}
