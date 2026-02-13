//
// Created by Ben Meyers on 2/12/26.
//

#ifndef RELATIVITY_IMAGE_H
#define RELATIVITY_IMAGE_H
#include <SDL3/SDL.h>

#include "Transform.h"

class Image {
    Transform mTransform;
    SDL_Texture* mTexture = nullptr;
public:
    void Draw(SDL_Renderer* renderer);
    void DrawWithPivot(SDL_Renderer* renderer, SDL_FPoint pivot);
    void SetTexture(SDL_Texture* texture);
    Transform& GetTransform(){return mTransform;}
};


#endif //RELATIVITY_IMAGE_H