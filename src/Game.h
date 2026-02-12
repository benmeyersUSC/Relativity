//
// Created by Ben Meyers on 2/11/26.
//

#ifndef RELATIVITY_GAME_H
#define RELATIVITY_GAME_H

#pragma once

#include "SDL3/SDL.h"
#include "Math.h"
#include <functional>
#include <vector>

#include "Actor.h"
using std::vector;
using Math::Vector2;
class Paddle;

class Game
{
	static constexpr float DURATION_SECONDS = 8.0f;
	static constexpr unsigned ESTIMATED_FPS = 60;

	static constexpr float PLOT_POINT_SIZE = 4.0f;
public:
	// window constants
	static constexpr float WINDOW_WIDTH = 1000.0f;
	static constexpr float WINDOW_HEIGHT = 800.0f;

	// max deltatime for update
	static constexpr float MAX_DELTA_TIME = 0.033f;

	// constant for full color
	static constexpr Uint8 MAX_COLOR = 255;

	// constant for half size of character pixel count (according to SDL)
	static constexpr float HALF_CHAR_PIXELS = 4.0f;

	// constant for 1000 ms in one s
	static constexpr float MS_PER_SEC = 1000.0f;

	// just covering all my bases w these contsants!
	static constexpr float FLIP_SIGN = -1.0f;
	static constexpr float HALF_DIVISOR = 2.0f;

	static constexpr float MAX_VELO = 1000.0f;

	Game();

	// Initialize the game
	// Returns true if successful
	bool Initialize();

	// Runs an interation of the game loop
	// Returns true if the game loop should continue
	bool RunIteration();

	// Called when the game gets shutdown
	void Shutdown();

	// Called when the game receives an event from SDL
	void HandleEvent(const SDL_Event* event);

	template<typename A>
	A* CreateActor() {
		// create actor on heap, save pointer to vector
		A* a = new A(static_cast<unsigned>(DURATION_SECONDS * ESTIMATED_FPS));
		mActors.push_back(a);
		return a;
	}

private:
	// window and renderer
	SDL_Window* mSdlWindow;
	SDL_Renderer* mSdlRenderer;

	// keep the game going
	bool mContinueRunning;
	bool mGameDone = false;

	// aggregation factor, computed after simulation ends
	unsigned mFrameAgg = 1;

	// actors vector and individual member variables
	vector<Actor*> mActors;
	Paddle* mPaddle;
	std::vector<float> mActorPositions;
	std::vector<float> mActorVelocities;

	// prev time for delta calcs
	Uint64 mPreviousTime;

	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	void LoadData();
	void UnloadData();

	void EndGame();

	void TransformPoints(const std::vector<float>& src, std::vector<float>& dest, const std::function<float(float)>& transformFunc);
	// these should return [-WIDTH/2, WIDTH/2]
	std::function<float(float)> mTransformPosition = [](const float position) {
		return position - WINDOW_WIDTH/2.0f;
	};
	std::function<float(float)> mTransformVelocity = [](const float velo) {
		// rel of max velo, scaled to WIDTH/2
		return (velo * WINDOW_WIDTH) / (MAX_VELO * 2.0f);
	};

	void DrawPaddleText();
	void DrawSpacetime();


	float mDT = 0.0f;
};

extern Game gGame;

#endif //RELATIVITY_GAME_H