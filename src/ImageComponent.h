//
// Created by Ben Meyers on 2/13/26.
//

#ifndef RELATIVITY_IMAGECOMPONENT_H
#define RELATIVITY_IMAGECOMPONENT_H
#include "Component.h"
#include <SDL3/SDL.h>

#include "Transform.h"
class ImageComponent : public  Component{
    Transform mTransform;
    SDL_Texture* mTexture = nullptr;
public:
    void Draw(SDL_Renderer* renderer);
    void DrawWithPivot(SDL_Renderer* renderer, SDL_FPoint pivot);
    void SetTexture(SDL_Texture* texture);
    Transform& GetTransform(){return mTransform;}
protected:
    explicit ImageComponent(class Actor *owner);
    void HandleUpdate(float deltaTime) override;

    friend class Actor;
};


#endif //RELATIVITY_IMAGECOMPONENT_H