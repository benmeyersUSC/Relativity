//
// Created by Ben Meyers on 2/13/26.
//
#pragma once
#include "Component.h"
#include <vector>

class SpacetimeComponent : public Component {
public:
	[[nodiscard]] std::vector<float>& GetPositions() {return mPositions;}
	[[nodiscard]] std::vector<float>& GetVelocities() {return mVelocities;}
	[[nodiscard]] std::vector<float>& GetTimeVelocities() {return mTimeVelocities;}
	[[nodiscard]] float GetVelocity()const{return mVelo;}
	float& GetVelocity(){return mVelo;}
	[[nodiscard]] float GetTimeFactor()const;
	void Setup(size_t frameLife);
	[[nodiscard]] float LifePct()const;
protected:
	explicit SpacetimeComponent(class Actor *owner);
	void HandleUpdate(float deltaTime) override;

	friend class Actor;
private:
	float mVelo = 0.0f;
	std::vector<float> mPositions;
	std::vector<float> mVelocities;
	std::vector<float> mTimeVelocities;
};
