//
// Created by Ben Meyers on 2/11/26.
//

#ifndef RELATIVITY_ACTOR_H
#define RELATIVITY_ACTOR_H

#include "Transform.h"
class Actor {
public:
    Transform& GetTransform() { return mTransform; }
    const Transform& GetTransform() const { return mTransform; }

protected:
    Transform mTransform;
};


#endif //RELATIVITY_ACTOR_H