//
// Created by Ben Meyers on 2/11/26.
//

#ifndef RELATIVITY_GAME_H
#define RELATIVITY_GAME_H


#pragma once

#include "SDL3/SDL.h"
#include "Math.h"

#include <vector>
using std::vector;

class Actor;

class Game
{
public:
	// window constants
	static constexpr float WINDOW_WIDTH = 800.0f;
	static constexpr float WINDOW_HEIGHT = 600.0f;

	// some actor size/speed constants
	static constexpr float WALL_WIDTH = 15.0f;
	static constexpr float PADDLE_WIDTH = 100.0f;
	static constexpr float PADDLE_SPEED = 300.0f;

	// max deltatime for update
	static constexpr float MAX_DELTA_TIME = 0.033f;

	// base speed for ball velo
	static constexpr float BALL_SPEED = 100.0f;
	static constexpr float BALL_SPEED_MULT = 1.25f;

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

	Actor* CreateActor();

private:
	// window and renderer
	SDL_Window* mSdlWindow;
	SDL_Renderer* mSdlRenderer;

	// keep the game going
	bool mContinueRunning;

	// actors vector and individual member variables
	vector<Actor*> mActors;
	Actor* mBall;
	Actor* mLeftWall;
	Actor* mRightWall;
	Actor* mTopWall;
	Actor* mPaddle;

	// prev time for delta calcs
	Uint64 mPreviousTime;

	// will be +/- 1 or 0, defaulted and always reset to 0 after update
	float mPaddleMoveDir;

	// ball velo
	Vector2 mBallVelo;

	// keeping track of score!
	unsigned mScore;

	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	void LoadData();
	void UnloadData();

	void MovePaddle(float deltaTime);
	void MoveBall(float deltaTime);
	void BallCollision();
};

extern Game gGame;

#endif //RELATIVITY_GAME_H