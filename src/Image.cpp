//
// Created by Ben Meyers on 2/12/26.
//

#include "Image.h"

void Image::Draw(SDL_Renderer* renderer)
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

    SDL_RenderTextureRotated(renderer, mTexture, nullptr, &rect, -mTransform.GetRotation(), nullptr,
                             SDL_FLIP_NONE);
}

void Image::SetTexture(SDL_Texture* texture)
{
    mTexture = texture;
}