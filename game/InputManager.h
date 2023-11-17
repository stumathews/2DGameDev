#pragma once
#include "GameCommands.h"
#include "input/IInputManager.h"

class InputManager final : public gamelib::IInputManager
{
public:
	explicit InputManager(std::shared_ptr<GameCommands> gameCommands, const int sampleRatePerSec, const bool verbose)
	: samplePerSec(sampleRatePerSec), gameCommands(std::move(gameCommands)), verbose(verbose) {}
	
	void Sample(unsigned long deltaMs) override;
	int GetSampleRatePerSec() override;

private:

	int samplePerSec;
	std::shared_ptr<GameCommands> gameCommands;
	const bool verbose;
	unsigned long accumulatedTimeMs{};
	long sampleCount {};
};

