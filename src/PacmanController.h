#pragma once

#include "Controller.h"
#include "BehaviorTree.h"
#include "BTGhostController.h"

class PacmanController : public Controller {

private:

	std::shared_ptr<Selector> root;

public:

	PacmanController(
		std::shared_ptr<Character> character
	);

	virtual ~PacmanController();

	virtual Move getMove(
		const GameState& game
	) override;
};


// CONDITIONS

class GhostNearby : public Behavior {

public:

	virtual Status update() override;
};


// ACTIONS


class Escape : public Behavior {

public:

	virtual Status update() override;
};

class SeekPill : public Behavior {

public:

	virtual Status update() override;
};

class RandomMoveAction : public Behavior {

public:

	virtual Status update() override;
};