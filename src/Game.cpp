#include "Game.h"

#include <iostream>

#include "Actor.h"
#include "Paddle.h"
#include <functional>

#include "SpacetimePlot.h"
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

	// mouse
	float x = 0.0f;
	float y = 0.0f;
	SDL_MouseButtonFlags mouseButtons = SDL_GetMouseState(&x, &y);
	mMousePos = {x, y};

	for (auto a : mActors) {
		a->HandleInput(keyboardState, mouseButtons, mMousePos);
	}
}

void Game::TransformPoints(const std::vector<float>& src, std::vector<float>& dest, const std::function<float(float)>& transformFunc, bool notTime) const {
	for (size_t i = 0; i + mFrameAgg <= src.size(); i += mFrameAgg) {
		float sum = 0.0f;
		for (size_t x = 0; x < mFrameAgg; ++x) {
			sum += src[i + x];
		}
		// now these are transformed for rendering
		dest.push_back((transformFunc(sum/(static_cast<float>(mFrameAgg) * 1.0f)) + static_cast<float>(notTime) * HALF_WIDTH));
	}
}

void Game::DrawFloat(float x, float y, const char* fmt, float value, float scale) {
	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR,MAX_COLOR,MAX_COLOR,MAX_COLOR);
	if (scale != 1.0f) { SDL_SetRenderScale(mSdlRenderer, scale, scale); }
	SDL_RenderDebugTextFormat(mSdlRenderer, x / scale, y / scale, fmt, value);
	if (scale != 1.0f) { SDL_SetRenderScale(mSdlRenderer, 1.0f, 1.0f); }
}

void Game::DrawInt(float x, float y, int value, float scale) {
	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR,MAX_COLOR,MAX_COLOR,MAX_COLOR);
	if (scale != 1.0f) { SDL_SetRenderScale(mSdlRenderer, scale, scale); }
	SDL_RenderDebugTextFormat(mSdlRenderer, x / scale, y / scale, "%i", value);
	if (scale != 1.0f) { SDL_SetRenderScale(mSdlRenderer, 1.0f, 1.0f); }
}

void Game::DrawPaddleText() {
	if (mPaddle == nullptr){return;}
	// now we type in the score onto the paddle
	Vector2 paddleRect = mPaddle->GetTransform().GetPosition();
	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR, MAX_COLOR, MAX_COLOR, MAX_COLOR);
	
	// time remaining
	DrawInt(paddleRect.x - HALF_CHAR_PIXELS, paddleRect.y - 10.0f / HALF_CHAR_PIXELS, static_cast<int>(DURATION_SECONDS - mDT), 1.35f);

	// pos + velo scaled 2x
	float posY = paddleRect.y - 10.0f / HALF_CHAR_PIXELS - mPaddle->GetTransform().GetSize().y * 2;
	float veloY = paddleRect.y - 10.0f / HALF_CHAR_PIXELS - mPaddle->GetTransform().GetSize().y;
	DrawFloat(paddleRect.x - CHAR_PIXELS*2, posY, "%.0f", mPaddle->GetTransform().GetPosition().x - HALF_WIDTH, 2.0f);
	DrawFloat(paddleRect.x - CHAR_PIXELS*2, veloY, "%.0f", mPaddle->GetVelocity(), 2.0f);
}


void Game::EndGame() {
	mSpacetimePlot = std::make_unique<SpacetimePlot>();


	unsigned numSamples = mPaddle->GetPositions().size();
	auto maxPlotPoints = static_cast<unsigned>(WINDOW_HEIGHT / PLOT_POINT_SIZE);
	// how many samples will we average to meet the ideal/max plot points
	mFrameAgg = std::max(1u, numSamples/ maxPlotPoints);

	// transform raw samples
	// - scaled by width
	// - mapped via aggregation to vertical plotting space!
	TransformPoints(mPaddle->GetPositions(), mSpacetimePlot->GetPositions(), mTransformPosition);
	TransformPoints(mPaddle->GetVelocities(), mSpacetimePlot->GetVelocities(), mTransformVelocity);
	TransformPoints(mPaddle->GetTimeVelocities(), mSpacetimePlot->GetTimeVelocities(), mTransformTimeVelocity, false);

	// destroy actors and ditch paddle
	UnloadData();
	mPaddle = nullptr;
}

void Game::UpdateGame()
{
	// calculate deltatime
	Uint64 currTimeMs = SDL_GetTicks();
	Uint64 uIntDiff = currTimeMs - mPreviousTime;
	mPreviousTime = currTimeMs;
	float deltaTime = static_cast<float>(uIntDiff) / MS_PER_SEC;
	deltaTime = Math::Min(MAX_DELTA_TIME, deltaTime);
	mDT += deltaTime;

	// 'leading edge' of elapsed eclipsing set duration
	if (mDT >= DURATION_SECONDS && !mGameDone) {
		mGameDone = true;
		return EndGame();
	}

	for (auto a : mActors) {
		a->HandleUpdate(deltaTime);
	}
}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(mSdlRenderer, 0, 0, 0, MAX_COLOR);
	SDL_RenderClear(mSdlRenderer);

	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR,MAX_COLOR,MAX_COLOR,MAX_COLOR);
	for (size_t i = 0; i < WINDOW_WIDTH; ++i) {
		SDL_FRect rect(i - 1.0f, HALF_HEIGHT - 1.0f, 2.0f, 2.0f);
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
		mSpacetimePlot->DrawSpacetime();
	}
	else	{

		DrawPaddleText();
	}

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

