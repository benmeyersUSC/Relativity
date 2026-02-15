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
public:
    void Draw();
    void DrawWithPivot(SDL_FPoint pivot);
    void SetTexture(SDL_Texture* texture);
    [[nodiscard]] Transform& GetTransform() const;
protected:
    explicit ImageComponent(class Actor *owner);
    void HandleRender() override;

    friend class Actor;
};


#endif //RELATIVITY_IMAGECOMPONENT_H