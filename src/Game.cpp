#include "Game.h"
#include "Actor.h"

Game gGame;

Game::Game()
{
	// comments in class declaration explain these members

	mSdlWindow = nullptr;
	mSdlRenderer = nullptr;

	mContinueRunning = true;

	// actors start null
	mBall = nullptr;
	mLeftWall = nullptr;
	mRightWall = nullptr;
	mTopWall = nullptr;
	mPaddle = nullptr;

	mPreviousTime = 0;

	mPaddleMoveDir = 0;

	// use the base ball speed (positive in both, arbitrary)
	mBallVelo = Vector2(BALL_SPEED, BALL_SPEED);

	mScore = 0;
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

	// I have included the ability to move with the arrow keys (can easily remove this)
	// paddle moves are independent, so when you hold both, the net effect is 0

	// SDL_SCANCODE_LEFT = 80,
	// A = 4
	if (keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT])
	{
		mPaddleMoveDir -= PADDLE_SPEED;
	}
	// D = 7
	// SDL_SCANCODE_RIGHT = 79,
	if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT])
	{
		mPaddleMoveDir += PADDLE_SPEED;
	}
}

void Game::MovePaddle(float deltaTime)
{
	// update paddle Transform with new Vector2, updated according to deltatime and move dir

	Vector2 paddlePos = mPaddle->GetTransform().GetPosition();
	// add change to x (could be negative)
	float newX = paddlePos.X() + deltaTime * mPaddleMoveDir;
	// ensure it's still on screen
	newX = Math::Max( // max of farthest_left_pos and valid_pos_to_the_right
		PADDLE_WIDTH /
			HALF_DIVISOR, // valid_pos_to_the_right is just min of newX and farthest_right_pos
		Math::Min(WINDOW_WIDTH - PADDLE_WIDTH / HALF_DIVISOR, newX));
	// update paddle
	mPaddle->GetTransform().SetPosition(Vector2(newX, paddlePos.Y()));
	// reset move dir to 0
	mPaddleMoveDir = 0;
}

void Game::BallCollision()
{
	// capture actor rects (for ease-of- and repeat-use)
	SDL_FRect ball = mBall->GetTransform().GetRect();
	SDL_FRect leftWall = mLeftWall->GetTransform().GetRect();
	SDL_FRect rightWall = mRightWall->GetTransform().GetRect();
	SDL_FRect topWall = mTopWall->GetTransform().GetRect();
	SDL_FRect paddle = mPaddle->GetTransform().GetRect();

	// either side wall flips x direction (if ball is moving perpendicular)
	if ((SDL_HasRectIntersectionFloat(&ball, &leftWall) && mBallVelo.x < 0.0) ||
		(SDL_HasRectIntersectionFloat(&ball, &rightWall) && mBallVelo.x > 0.0))
	{
		mBallVelo.x *= FLIP_SIGN;
	}
	// likewise here, make sure ball is indeed perpendicular before reflecting
	if ((SDL_HasRectIntersectionFloat(&ball, &topWall) && mBallVelo.y < 0.0) ||
		(SDL_HasRectIntersectionFloat(&ball, &paddle) && mBallVelo.y > 0.0))
	{
		// if it is indeed the paddle, increase the ball speed and increase score!
		if (SDL_HasRectIntersectionFloat(&ball, &paddle))
		{
			// speed the ball up every paddle hit
			mBallVelo *= BALL_SPEED_MULT; // thank goodness for operator overload!
			mScore++;
		}
		// regardless, reflect y
		mBallVelo.y *= FLIP_SIGN;
	}
}

void Game::MoveBall(float deltaTime)
{
	Vector2 newBall = mBall->GetTransform().GetPosition() +
					  Vector2(deltaTime * mBallVelo.X(), deltaTime * mBallVelo.Y());
	mBall->GetTransform().SetPosition(newBall);

	// handle ball collisions separately
	BallCollision();

	// if ball's y coordinate is below (higher nominally) than height, you lose
	if (mBall->GetTransform().GetPosition().Y() > WINDOW_HEIGHT)
	{
		mContinueRunning = false;
	}
}

void Game::UpdateGame()
{
	// calculate deltatime
	Uint64 currTimeMs = SDL_GetTicks();
	Uint64 uIntDiff = currTimeMs - mPreviousTime;
	mPreviousTime = currTimeMs;
	float deltaTime = uIntDiff / MS_PER_SEC;
	deltaTime = Math::Min(MAX_DELTA_TIME, deltaTime);

	// use deltatoem to update ball and paddle

	// paddle
	MovePaddle(deltaTime);
	// ball
	MoveBall(deltaTime);
}

void Game::GenerateOutput()
{
	// lowkey white background
	SDL_SetRenderDrawColor(mSdlRenderer, MAX_COLOR, MAX_COLOR, MAX_COLOR, MAX_COLOR);
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
	SDL_RenderDebugTextFormat(mSdlRenderer, paddleRect.X() - HALF_CHAR_PIXELS,
							  paddleRect.Y() - WALL_WIDTH / HALF_CHAR_PIXELS, "%i", mScore);
	SDL_RenderPresent(mSdlRenderer);
}

void Game::LoadData()
{
	// re-used, so let's store these
	float halfWidth = WINDOW_WIDTH / HALF_DIVISOR;
	float halfHeight = WINDOW_HEIGHT / HALF_DIVISOR;
	float halfWallWidth = WALL_WIDTH / HALF_DIVISOR;

	// could try to make this one big loop, but would need other branching or datastructures...
	// figured it makes sense to do like this because all the objects are different
	// and they're all getting their own member variables

	mBall = CreateActor(); // 15 x 15
	mBall->GetTransform().SetSize(Vector2(WALL_WIDTH, WALL_WIDTH));
	mBall->GetTransform().SetPosition(Vector2(halfWidth, halfHeight));

	mLeftWall = CreateActor(); // 15 thick, as tall as window
	mLeftWall->GetTransform().SetSize(Vector2(WALL_WIDTH, WINDOW_HEIGHT));
	// middle is 7.5		half height
	mLeftWall->GetTransform().SetPosition(Vector2(halfWallWidth, halfHeight));

	mRightWall = CreateActor(); // 15 thick, as tall as window
	mRightWall->GetTransform().SetSize(Vector2(WALL_WIDTH, WINDOW_HEIGHT));
	//        middle is WIDTH - 7.5	     half height
	mRightWall->GetTransform().SetPosition(Vector2(WINDOW_WIDTH - halfWallWidth, halfHeight));

	mTopWall = CreateActor(); // as wide as window, 15 tall (thick?)
	mTopWall->GetTransform().SetSize(Vector2(WINDOW_WIDTH, WALL_WIDTH));
	//   middle width			7.5 from top
	mTopWall->GetTransform().SetPosition(Vector2(halfWidth, halfWallWidth));

	mPaddle = CreateActor();
	mPaddle->GetTransform().SetSize(Vector2(PADDLE_WIDTH, WALL_WIDTH)); // not quite bottom
	mPaddle->GetTransform().SetPosition(Vector2(halfWidth, WINDOW_HEIGHT - WALL_WIDTH));
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

Actor* Game::CreateActor()
{
	// create actor on heap, save pointer to vector
	Actor* a = new Actor();
	mActors.push_back(a);
	return a;
}
