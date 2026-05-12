#pragma once

#include "Controller.h"
#include "BlinkyStateMachine.h"

class BlinkyController : public Controller {

private:

	std::shared_ptr<BlinkyStateMachine> fsm;

public:

	BlinkyController(
		std::shared_ptr<Character> character
	);

	virtual ~BlinkyController();

	virtual Move getMove(
		const GameState& game
	) override;
};