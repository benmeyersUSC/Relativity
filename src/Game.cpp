#include "Game.h"

#include <iostream>

#include "Actor.h"
#include "Paddle.h"
#include <functional>
#include <string>
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

	mActorPositions.reserve(static_cast<unsigned>(WINDOW_HEIGHT / PLOT_POINT_SIZE));
	mActorVelocities.reserve(static_cast<unsigned>(WINDOW_HEIGHT / PLOT_POINT_SIZE));
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
	DrawFloat(paddleRect.x - CHAR_PIXELS*2, posY, "%.0f", mPaddle->GetTransform().GetPosition().x, 2.0f);
	DrawFloat(paddleRect.x - CHAR_PIXELS*2, veloY, "%.0f", mPaddle->GetVelocity(), 2.0f);
}

void Game::DrawSpacetime() {
	// plot as many positions fit in the window (we'll accept overlap)
	float yStep = mActorPositions.empty() ? PLOT_POINT_SIZE
	              : WINDOW_HEIGHT / static_cast<float>(mActorPositions.size());
	float y = WINDOW_HEIGHT;

	// Y (time) axis
	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR,MAX_COLOR,MAX_COLOR,MAX_COLOR);
	for (size_t i = 0; i < WINDOW_HEIGHT; ++i) {
		SDL_FRect rect(HALF_WIDTH - 1.0f, i - 1.0f, 2.0f, 2.0f);
		SDL_RenderFillRect(mSdlRenderer, &rect);
	}

	// how many time dilation prints do we want
	auto numTimePrints = 18.0f;
	auto timePrintInterval = static_cast<unsigned>(static_cast<float>(mActorPositions.size()) / numTimePrints);

	for (size_t i = 0; i < mActorPositions.size(); ++i) {
		// pos, velo, timeDil for plotting
		float mSpacetimePos = mActorPositions[i];
		float mSpacetimeVelo = mActorVelocities[i];
		// mul by two to make it brighter...magic
		float veloPct = Math::Abs(2.0f * (mSpacetimeVelo - HALF_WIDTH)) / HALF_WIDTH;

		// for printing dots and text
		bool neg = mSpacetimeVelo < HALF_WIDTH;
		unsigned r = neg * MAX_COLOR;
		unsigned g = !neg * MAX_COLOR;
		auto alpha = static_cast<unsigned>(MAX_COLOR * veloPct);

		// pos - white dots
		SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR, MAX_COLOR, MAX_COLOR, MAX_COLOR);
		SDL_FRect rect(mSpacetimePos - HALF_PLOT_POINT, y - HALF_PLOT_POINT, PLOT_POINT_SIZE, PLOT_POINT_SIZE);
		SDL_RenderFillRect(mSdlRenderer, &rect);

		// print the time dilation at various times
		if (i % timePrintInterval == 0 && i != 0){
			bool negativePosition = mSpacetimePos < HALF_WIDTH;
			float shiftMag = 30.0f;
			float shift = static_cast<float>(negativePosition - !negativePosition) * (shiftMag * PLOT_POINT_SIZE);
			DrawFloat(mSpacetimePos - HALF_PLOT_POINT - HALF_CHAR_PIXELS + shift,
							  y - HALF_PLOT_POINT, "%.2f%%", mActorTimeVelocities[i], 1.35f);
		}

		// velocity-gauged velo plotting
		// - color is direction-gauged
		// - transparency is magnitude-gauged
		SDL_SetRenderDrawBlendMode(mSdlRenderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(mSdlRenderer, r, g, 0, alpha);
		SDL_FRect rect2(mSpacetimeVelo - HALF_PLOT_POINT, y - HALF_PLOT_POINT, PLOT_POINT_SIZE, PLOT_POINT_SIZE);
		SDL_RenderFillRect(mSdlRenderer, &rect2);
		SDL_SetRenderDrawBlendMode(mSdlRenderer, SDL_BLENDMODE_NONE);

		y -= yStep;
	}

	// mouse-height(time)-gauged information
	// index is inverse height (sdl convention), quantized by datapoints, clamped at 0 and max-index
	auto mouseHeightIndex = static_cast<size_t>(std::clamp((WINDOW_HEIGHT - mMousePos.y) / yStep, 0.0f, static_cast<float>(mActorPositions.size() - 1)));
	float mouseTime = mActorTimeVelocities[mouseHeightIndex];
	// de-transforming values so centered in middle of plot
	float mousePos = mActorPositions[mouseHeightIndex] - HALF_WIDTH;
	float mouseVelo = mActorVelocities[mouseHeightIndex] - HALF_WIDTH;

	// total magic here--fix
	float textW = CHAR_PIXELS * 27;
	float textH = CHAR_PIXELS * 3 + 4.0f;
	// always put mouse 'inward' (x and y)
	float mouseTextX = mMousePos.x < HALF_WIDTH ? mMousePos.x + CHAR_PIXELS : mMousePos.x - textW - CHAR_PIXELS;
	float mouseTextY = mMousePos.y < HALF_HEIGHT ? mMousePos.y + CHAR_PIXELS : mMousePos.y - textH - CHAR_PIXELS;

	// outlined box
	float pad = 4.0f;
	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR, MAX_COLOR, MAX_COLOR, MAX_COLOR);
	SDL_FRect box(mouseTextX - pad, mouseTextY - pad, textW + pad * 2, textH + pad * 2);
	SDL_RenderRect(mSdlRenderer, &box);

	// draw text
	DrawFloat(mouseTextX, mouseTextY + (CHAR_PIXELS + 2.0f) * 2, "Space Position: %.1f", mousePos);
	DrawFloat(mouseTextX, mouseTextY + CHAR_PIXELS + 2.0f, "Space Velocity: %.1f", mouseVelo);
	DrawFloat(mouseTextX, mouseTextY, "Aging Factor: %.1f%%", mouseTime);
}

void Game::EndGame() {
	unsigned numSamples = mPaddle->GetPositions().size();
	auto maxPlotPoints = static_cast<unsigned>(WINDOW_HEIGHT / PLOT_POINT_SIZE);
	// how many samples will we average to meet the ideal/max plot points
	mFrameAgg = std::max(1u, numSamples/ maxPlotPoints);

	// transform raw samples
	// - scaled by width
	// - mapped via aggregation to vertical plotting space!
	TransformPoints(mPaddle->GetPositions(), mActorPositions, mTransformPosition);
	TransformPoints(mPaddle->GetVelocities(), mActorVelocities, mTransformVelocity);
	TransformPoints(mPaddle->GetTimeVelocities(), mActorTimeVelocities, mTransformTimeVelocity, false);

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
		DrawSpacetime();
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

