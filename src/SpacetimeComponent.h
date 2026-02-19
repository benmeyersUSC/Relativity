//
// Created by Ben Meyers on 2/13/26.
//
#pragma once
#include "Component.h"
#include <vector>

class SpacetimeComponent : public Component {
public:
	[[nodiscard]] std::vector<float>& GetSpatialPositions() {return mSpatialPositions;}
	[[nodiscard]] std::vector<float>& GetSpatialVelocities() {return mSpatialVelocities;}
	[[nodiscard]] std::vector<float>& GetProperTimes() {return mProperTimes;}
	float& GetSpatialVelocity(){return mSpatialVelo;}
	[[nodiscard]] float GetProperOverCoordinateTime()const;
	void Setup(size_t frameLife);
	[[nodiscard]] float ElapsedPct()const;
protected:
	explicit SpacetimeComponent(class Actor *owner);
	void HandleUpdate(float deltaTime) override;

	friend class Actor;
private:
	float mSpatialVelo = 0.0f;
	std::vector<float> mSpatialPositions;
	std::vector<float> mSpatialVelocities;
	std::vector<float> mProperTimes;
};
