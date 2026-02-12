//
// Created by Ben Meyers on 2/11/26.
//

#ifndef RELATIVITY_ACTOR_H
#define RELATIVITY_ACTOR_H
#include <vector>
#include "Transform.h"
#include "SDL3/SDL_mouse.h"

class Actor {
public:
    virtual ~Actor() = default;

    Transform& GetTransform() { return mTransform; }
    [[nodiscard]] const Transform& GetTransform() const { return mTransform; }

    explicit Actor( unsigned frameLife);
    virtual void HandleUpdate(float deltaTime);
    virtual void HandleInput(const bool keys[], SDL_MouseButtonFlags mouseButtons,
                             const Vector2& relativeMouse);

    [[nodiscard]] std::vector<float>& GetPositions() {return mPositions;}
     [[nodiscard]] std::vector<float>& GetVelocities() {return mVelocities;}
protected:
    Transform mTransform;
    float mVelo = 0.0f;
    std::vector<float> mPositions;
    std::vector<float> mVelocities;
};


#endif //RELATIVITY_ACTOR_H