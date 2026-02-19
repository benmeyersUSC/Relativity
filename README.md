# [RELATIVITY](https://benmeyersusc.github.io/Relativity/)


I want to display the invariance in velocity of mass's traversal 
through spacetime using some skills from my video game programming courses!

## Flow
- First, you can move the blue object/paddle along a one dimensional axis according to 
either A/D or </> controls. Your time remaining is shown on the blue box. 
  - You will be changing acceleration with these controls. Acceleration through spacetime, as in real life,
  is capped at a universal constant, ***C***. As you approach maximum spacetime
  velocity *in the spatial direction*, it will become asymptotically harder to 
  accelerate. Additionally, as you approach ***C***, your time dilation factor will increase
  and you will age ever more slowly...
  - At every frame in your 1-dimensional play, your position, velocity, and time-dilation factor
  will be recorded for later display. My favorite part of this codebase is in [SpacetimeComponent.cpp](./src/Actor.cpp)'s `HandleUpdate` 
  function, where we calculate the **Lorentz Factor** for time dilation. 
  - Here is the code:
  ```cpp
  mVelocities.push_back(mVelo);
    mPositions.push_back(GetOwner()->GetTransform().GetPosition().x);

    // LORENTZ
    // mVelo^2 + timeVelo^2 = MAX_VELO^2
    // timeVelo = SQRT(
    //      MAX_VELO^2 - mVelo^2
    // )
    mTimeVelocities.push_back(
        Math::Sqrt(
            Game::MAX_VELO * Game::MAX_VELO - mVelo * mVelo
        )
    );
  ```
  - Because your spacetime vector is **[SpaceVelocity, TimeVelocity]** and its length must equal ***C***, 
  we can set up the following equality based on the formula for a vector's length:
    - **SpaceVelocity^2** + **TimeVelocity^2** = **C^2**
  - This then gives that at any instant, your time velocity is a function of your space velocity:
    - **TimeVelocity** = sqrt(**C^2** - **SpaceVelocity^2**)
- Then once your time is up, you get to see the 2d spacetime diagram of your movement and aging!
  - Per convention, *time* is plotted vertically; then each point in this x-y plane represents a spot in
  spacetime. In white, you'll see your position across time; in red or green (depending on direction) you will see your
  spatial velocity. Finally, at several points advancing in time, you will see your aging factor. If it says
  **72.3%**, it means that at that velocity, your instantaneous rate of aging was **72.3%** of that of an object observing you from rest. 
  - For fine-grained analysis, as you move your mouse up and down through time, a box will let you know specific
  position, spatial velocity, and aging factor values. 

There is much more (such as a continuous display of your ***spacetime vector*** and its components as well as 3d generalization!)
to come...


...

Some more notes on discrete time dilation
```cpp
/*

proper time elapsed by mover:

coordinate (rest) DURATION * average_of(√(1 - (v_i / MAX_VELO)²))

this makes sense. coordinate duration * the average of (Z)...what is Z?

Z = √(1 - (v_i / MAX_VELO)²)
let's derive Z...first, recognize:
- v_i (velocity at period i) is dx_i/dt_i
- MAX_VELO is our 'speed of light' (c)
...

fundamental invariant in spacetime;
ds² = c²dt² - dx²
and
ds² = c²dτ²

s = spacetime interval; the 'distance' between two events in spacetime
t = coordinate time (measured by resting object)
x = spatial coordinate of moving object
c = speed of light or MAX_VELO
τ = proper time experienced by mover

so ds² (the squared change in spacetime) is made up of two competing terms:
- c²dt²
-- squared change in time, scaled by c for like units
-- c * dt = how far light travelled in dt! (if t were a year, we'd have lightyears)
- dx²
-- scaled spatial change
so the spacetime interval, s, is proportional to the difference between
how much light moves in some unit of time and how much the object moves

the ds² term is invariant; let's equate two equations:

ds² = c²dt² - dx²
and
ds² = c²dτ²

become:

c²dτ² = c²dt² - dx²
...
c²dτ² + dx² = c²dt²

dt is just the resting viewer's clock speed. it is unchanging and finite.
we have dx (mover's change in space) and c * dτ (mover's 'light-distance' in
their own experienced time).
as the mover moves more (higher velocity = dx/dt), the lower must be dτ
(the rate at which they experience time)


c²dτ² = c²dt² - dx²          // divide both sides by c²dt²
(dτ/dt)² = 1 - (dx/dt)²/c²   // recognize dx/dt = v
(dτ/dt)² = 1 - v²/c²
dτ/dt   = √(1 - v²/c²)       // that's Z

*/
```