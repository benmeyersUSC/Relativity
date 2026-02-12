#include "Game.h"

#include <iostream>

#include "Actor.h"
#include "Paddle.h"
#include <functional>
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

	mActorPositions.reserve(FRAME_LIFE/FRAME_AGG);
	mActorVelocities.reserve(FRAME_LIFE/FRAME_AGG);
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

	// mouse
	float x = 0.0f;
	float y = 0.0f;
	SDL_MouseButtonFlags mouseButtons = SDL_GetRelativeMouseState(&x, &y);
	Vector2 relativeMouse(x, y);

	for (auto a : mActors) {
		a->HandleInput(keyboardState, mouseButtons, relativeMouse);
	}
}

void Game::TransformPoints(const std::vector<float>& src, std::vector<float>& dest, const std::function<float(float)>& transformFunc) {
	for (size_t i = 0; i < src.size() - (FRAME_AGG - 1); i += FRAME_AGG) {
		float sum = 0.0f;
		for (size_t x = 0; x < FRAME_AGG; ++x) {
			sum += transformFunc(src[i + x]);
		}
		// now these are transformed for rendering
		dest.push_back((sum/(FRAME_AGG * 1.0f) + WINDOW_WIDTH/2.0f));
	}
}

void Game::DrawPaddleText() {
	// now we type in the score onto the paddle
	Vector2 paddleRect = mPaddle->GetTransform().GetPosition();
	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR, MAX_COLOR, MAX_COLOR, MAX_COLOR);

	// course website suggested this function:
	//									minus 4 because text is 8   subtract a quarter so middle 8 (of 15) is text
	SDL_RenderDebugTextFormat(mSdlRenderer, paddleRect.x - HALF_CHAR_PIXELS,
							  paddleRect.y - 10.0f / HALF_CHAR_PIXELS, "%i", mFramesElapsed);
}

void Game::DrawSpacetime() {

	float y = WINDOW_HEIGHT;
	float midX = WINDOW_WIDTH / 2.0f;

	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR,MAX_COLOR,MAX_COLOR,MAX_COLOR);
	for (size_t i = 0; i < WINDOW_HEIGHT; ++i) {
		SDL_FRect rect(midX - 1.0f, i - 1.0f, 2.0f, 2.0f);
		SDL_RenderFillRect(mSdlRenderer, &rect);
	}

	for (float mSpacetimePos : mActorPositions) {
		float invPctOfMax = (1.0f - Math::Abs(mSpacetimePos) / midX);
		unsigned alpha = 255 * invPctOfMax;
		bool neg = mSpacetimePos < midX;
		unsigned r = neg * 255 * invPctOfMax;
		unsigned g = (!neg && !Math::NearlyZero(mSpacetimePos)) * 255 * invPctOfMax;

		SDL_SetRenderDrawColor(mSdlRenderer, r, g, 0, alpha);
		SDL_FRect rect(mSpacetimePos - PLOT_POINT_SIZE/2.0f, y - PLOT_POINT_SIZE/2.0f, PLOT_POINT_SIZE, PLOT_POINT_SIZE);
		SDL_RenderFillRect(mSdlRenderer, &rect);
		y -= PLOT_POINT_SIZE;
	}
}

void Game::EndGame() {
	std::vector<float>& poss = mPaddle->GetPositions();
	std::vector<float>& velos = mPaddle->GetVelocities();
	std::cout << "Game done after " << mFramesElapsed << "frames!\n";
	for (size_t i = 0; i < FRAME_LIFE;++i ) {
		for (int j = 0; j < Math::Abs((velos[i]) / 10); ++j) {
			if (velos[i] < 0) {
				std::cout << "<";
			}
			else {
				std::cout << ">";
			}
		}
		std::cout << velos[i] << "\n";
	}
	TransformPoints(poss, mActorPositions, mTransformPosition);
	TransformPoints(velos, mActorVelocities, mTransformVelocity);

	UnloadData();
	mPaddle = nullptr;
}

void Game::UpdateGame()
{
	// plot VELO (x) vs time (y) !

	// and then at certain points in the vertical plot, plot a rotated vector!
	// use trig...straight up is at rest, then max velo is rightward pointing...

	mFramesElapsed++;
	if (mFramesElapsed > FRAME_LIFE && !mGameDone) {
		mGameDone = true;
		return EndGame();
	}

	// calculate deltatime
	Uint64 currTimeMs = SDL_GetTicks();
	Uint64 uIntDiff = currTimeMs - mPreviousTime;
	mPreviousTime = currTimeMs;
	float deltaTime = uIntDiff / MS_PER_SEC;
	deltaTime = Math::Min(MAX_DELTA_TIME, deltaTime);

	// use deltatime to update ball and paddle

	for (auto a : mActors) {
		a->HandleUpdate(deltaTime);
	}
}

void Game::GenerateOutput()
{
	float hw = WINDOW_WIDTH/2.0f;
	float hh = WINDOW_HEIGHT/2.0f;
	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR,MAX_COLOR,MAX_COLOR,MAX_COLOR);
	for (size_t i = 0; i < WINDOW_WIDTH; ++i) {
		SDL_FRect rect(i - 1.0f, hh - 1.0f, 2.0f, 2.0f);
		SDL_RenderFillRect(mSdlRenderer, &rect);
	}

	// now set to blue
	SDL_SetRenderDrawColor(mSdlRenderer, 0, 0, MAX_COLOR, MAX_COLOR);
	// render each actor
	for (auto& actor : mActors)
	{
		SDL_FRect rect = actor->GetTransform().GetRect();
		SDL_RenderFillRect(mSdlRenderer, &rect);
	}

	if (mGameDone) {
		DrawSpacetime();
	}
	else	{DrawPaddleText();}

	SDL_RenderPresent(mSdlRenderer);
}

void Game::LoadData()
{
	mPaddle = CreateActor<Paddle>();
	mPaddle->GetTransform().SetPosition(Vector2(WINDOW_WIDTH / HALF_DIVISOR, WINDOW_HEIGHT / HALF_DIVISOR ));
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

