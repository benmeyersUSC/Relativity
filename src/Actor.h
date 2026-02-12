//
// Created by Ben Meyers on 2/11/26.
//

#ifndef RELATIVITY_ACTOR_H
#define RELATIVITY_ACTOR_H
#include <vector>
#include "Transform.h"

class Actor {
public:
    virtual ~Actor() = default;

    Transform& GetTransform() { return mTransform; }
    [[nodiscard]] const Transform& GetTransform() const { return mTransform; }

    explicit Actor( unsigned frameLife);
    virtual void HandleUpdate(float deltaTime);
    virtual void HandleInput(const bool* keyboardInput);

     [[nodiscard]] std::vector<float>& GetVelos() {return mVelos;}
protected:
    Transform mTransform;
    float mVelo = 0.0f;
    std::vector<float> mVelos;
};


#endif //RELATIVITY_ACTOR_H