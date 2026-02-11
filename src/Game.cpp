#include "Game.h"

#include <iostream>

#include "Actor.h"
#include "Paddle.h"

Game gGame;

Game::Game()
{
	// comments in class declaration explain these members

	mSdlWindow = nullptr;
	mSdlRenderer = nullptr;

	mContinueRunning = true;

	// actors start null
	mPaddle = nullptr;

	mPreviousTime = 0;

}

bool Game::Initialize()
{
	SDL_SetHint("SDL_MAIN_CALLBACK_RATE", "60");
	bool sdlInit = SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
	// fail if SDL init doesn't work
	if (!sdlInit)
	{
		return false;
	}
	// use window constants
	mSdlWindow = SDL_CreateWindow("PONG", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (mSdlWindow == nullptr)
	{
		return false;
	}

	mSdlRenderer = SDL_CreateRenderer(mSdlWindow, nullptr);
	if (mSdlRenderer == nullptr)
	{
		return false;
	}

	// init actors!
	LoadData();
	return true;
}

bool Game::RunIteration()
{
	// game loop!

	// get input
	ProcessInput();
	// update game objects
	UpdateGame();
	// render
	GenerateOutput();
	return mContinueRunning;
}

void Game::Shutdown()
{
	// call proper unloaders/destroyers
	UnloadData();
	SDL_DestroyRenderer(mSdlRenderer);
	SDL_DestroyWindow(mSdlWindow);
	SDL_Quit();
}

void Game::HandleEvent(const SDL_Event* event)
{
	// x button
	if (event->type == SDL_EVENT_QUIT)
	{
		mContinueRunning = false;
	}
}

void Game::ProcessInput()
{
	const bool* keyboardState = SDL_GetKeyboardState(nullptr);
	// esc key can also end game
	if (keyboardState[SDL_SCANCODE_ESCAPE])
	{
		mContinueRunning = false;
	}

	for (auto a : mActors) {
		a->HandleInput(keyboardState);
	}
}


void Game::UpdateGame()
{
	// plot VELO (x) vs time (y) !

	// and then at certain points in the vertical plot, plot a rotated vector!
	// use trig...straight up is at rest, then max velo is rightward pointing...

	mFramesElapsed++;
	if (mFramesElapsed > FRAME_LIFE) {
		mContinueRunning = false;
		std::cout << "Game done after " << mFramesElapsed << "frames!\n";
		auto x = mPaddle->GetVelos();
		for (size_t i = 0; i < FRAME_LIFE;++i ) {
			for (int j = 0; j < Math::Abs((x[i]) / 100); ++j) {
				if (x[i] < 0) {
					std::cout << "<";
				}
				else {
					std::cout << ">";
				}
			}
			std::cout << x[i] << "\n";
		}
		return;
	}

	// calculate deltatime
	Uint64 currTimeMs = SDL_GetTicks();
	Uint64 uIntDiff = currTimeMs - mPreviousTime;
	mPreviousTime = currTimeMs;
	float deltaTime = uIntDiff / MS_PER_SEC;
	deltaTime = Math::Min(MAX_DELTA_TIME, deltaTime);

	// use deltatoem to update ball and paddle

	for (auto a : mActors) {
		a->HandleUpdate(deltaTime);
	}
}

void Game::GenerateOutput()
{
	// lowkey white background
	SDL_SetRenderDrawColor(mSdlRenderer, 0,0,0,0);
	SDL_RenderClear(mSdlRenderer);
	// now set to blue
	SDL_SetRenderDrawColor(mSdlRenderer, 0, 0, MAX_COLOR, MAX_COLOR);
	// render each actor
	for (auto& actor : mActors)
	{
		SDL_FRect rect = actor->GetTransform().GetRect();
		SDL_RenderFillRect(mSdlRenderer, &rect);
	}

	// now we type in the score onto the paddle
	Vector2 paddleRect = mPaddle->GetTransform().GetPosition();
	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR, MAX_COLOR, MAX_COLOR, MAX_COLOR);

	// course website suggested this function:
	//									minus 4 because text is 8   subtract a quarter so middle 8 (of 15) is text
	SDL_RenderDebugTextFormat(mSdlRenderer, paddleRect.x - HALF_CHAR_PIXELS,
							  paddleRect.y - 10.0f / HALF_CHAR_PIXELS, "%i", mFramesElapsed);
	SDL_RenderPresent(mSdlRenderer);
}

void Game::LoadData()
{
	// re-used, so let's store these
	float halfWidth = WINDOW_WIDTH / HALF_DIVISOR;
	float halfHeight = WINDOW_HEIGHT / HALF_DIVISOR;


	mPaddle = CreateActor<Paddle>();
	mPaddle->GetTransform().SetPosition(Vector2(halfWidth, halfHeight ));

}

void Game::UnloadData()
{
	// delete objects and clear vector
	for (auto& actor : mActors)
	{
		delete actor;
	}
	mActors.clear();
}

