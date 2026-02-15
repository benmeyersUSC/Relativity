//
// Created by Ben Meyers on 2/13/26.
//

#ifndef RELATIVITY_IMAGECOMPONENT_H
#define RELATIVITY_IMAGECOMPONENT_H
#include "Component.h"
#include <SDL3/SDL.h>
#include "Transform.h"

class ImageComponent : public  Component{
    SDL_Texture* mTexture = nullptr;
    SDL_Renderer* mRenderer = nullptr;
    Transform mLocalTransform;
    bool mUseLocalTransform = false;
    bool mHasPivot = false;
    SDL_FPoint mPivot{};
public:
    void Draw();
    void DrawWithPivot(SDL_FPoint pivot);
    void SetTexture(SDL_Texture* texture);
    void SetUseLocalTransform(bool use) { mUseLocalTransform = use; }
    void SetPivot(SDL_FPoint pivot) { mHasPivot = true; mPivot = pivot; }
    [[nodiscard]] Transform& GetTransform();
protected:
    explicit ImageComponent(class Actor *owner);
    void HandleRender() override;

    friend class Actor;
};


#endif //RELATIVITY_IMAGECOMPONENT_H