#ifndef ANIMATION_HANDLER_H
#define ANIMATION_HANDLER_H

#include <SFML/Graphics.hpp>
#include <vector>

class Animation
{
public:

	unsigned int startFrame;
	unsigned int endFrame;

	float duration;

	Animation(unsigned int startFrame, unsigned int endFrame, float duration)
	{
		this->startFrame = startFrame;
		this->endFrame = endFrame;
		this->duration = duration;
	}

	unsigned int getLength() { return endFrame - startFrame + 1; }
};

class AnimationHandler
{
private:

	/* Array of animations */
	std::vector<Animation> animations;

	/* Current time since the animation loop started */
	float t;

	int currentAnim;

	bool paused = false;

public:

	void reset();

	/* pause / unpause the anim */
	bool isPaused() { return paused; }
	void pause() { if(!paused)paused = true; }
	void unPause() { if (paused)paused = false; }

	/* Add a new animation */
	void addAnim(Animation& anim);

	/* Update the current frame of animation. dt is the time since
	* the update was last called (i.e. the time for one frame to be
	* executed) */
	void update(const float dt);

	/* Change the animation, resetting t in the process */
	void changeAnim(unsigned int animNum);

	/* Current section of the texture that should be displayed */
	sf::IntRect bounds;

	/* Pixel dimensions of each individual frame */
	sf::IntRect frameSize;

	/* Constructor */
	AnimationHandler()
	{
		this->t = 0.0f;
		this->currentAnim = -1;
	}
	AnimationHandler(const sf::IntRect& frameSize)
	{
		this->frameSize = frameSize;
		this->paused = false;
		this->t = 0.0f;
		this->currentAnim = -1;
	}
};

#endif /* ANIMATION_HANDLER_H */