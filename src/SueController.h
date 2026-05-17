#pragma once

#include "Controller.h"
#include "SueStateMachine.h"

class SueController : public Controller {

private:

	std::shared_ptr<SueStateMachine> fsm;

public:

	SueController(
		std::shared_ptr<Character> character
	);

	virtual ~SueController();

	virtual Move getMove(
		const GameState& game
	) override;
};