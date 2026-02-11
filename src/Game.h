//
// Created by Ben Meyers on 2/11/26.
//

#ifndef RELATIVITY_GAME_H
#define RELATIVITY_GAME_H

#pragma once

#include "SDL3/SDL.h"
#include "Math.h"

#include <vector>

#include "Actor.h"
using std::vector;
using Math::Vector2;
class Paddle;

class Game
{
	static constexpr unsigned FRAME_LIFE = 1000;
public:
	// window constants
	static constexpr float WINDOW_WIDTH = 800.0f;
	static constexpr float WINDOW_HEIGHT = 600.0f;

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
		A* a = new A(FRAME_LIFE);
		mActors.push_back(a);
		return a;
	}

private:
	// window and renderer
	SDL_Window* mSdlWindow;
	SDL_Renderer* mSdlRenderer;

	// keep the game going
	bool mContinueRunning;

	// frames elapsed
	unsigned mFramesElapsed = 0;

	// actors vector and individual member variables
	vector<Actor*> mActors;
	Paddle* mPaddle;

	// prev time for delta calcs
	Uint64 mPreviousTime;

	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	void LoadData();
	void UnloadData();

};

extern Game gGame;

#endif //RELATIVITY_GAME_H