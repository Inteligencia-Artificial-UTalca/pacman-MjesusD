#ifndef MSPACMANCONTROLLER_H_
#define MSPACMANCONTROLLER_H_

#include "Controller.h"
#include "BehaviorTree.h"
#include "BTGhostController.h"

#include <memory>

///////////////////////////////////////////////////////////////////////////////
// CONDITIONS
///////////////////////////////////////////////////////////////////////////////

class MsGhostNearby : public Behavior {

public:

	Status update() override;
};

class MsEdibleGhostNearby : public Behavior {

public:

	Status update() override;
};

class MsPowerPillAvailable : public Behavior {

public:

	Status update() override;
};

///////////////////////////////////////////////////////////////////////////////
// ACTIONS
///////////////////////////////////////////////////////////////////////////////

class MsEscape : public Behavior {

public:

	Status update() override;
};

class MsEatGhost : public Behavior {

public:

	Status update() override;
};

class MsSeekPowerPill : public Behavior {

public:

	Status update() override;
};

class MsSeekPill : public Behavior {

public:

	Status update() override;
};

class MsRandomMoveAction : public Behavior {

public:

	Status update() override;
};

///////////////////////////////////////////////////////////////////////////////
// CONTROLLER
///////////////////////////////////////////////////////////////////////////////

class MsPacmanController : public Controller {

private:

	std::shared_ptr<Selector> root;

public:

	MsPacmanController(
		std::shared_ptr<Character> character
	);

	virtual ~MsPacmanController();

	Move getMove(
		const GameState& game
	) override;

	///////////////////////////////////////////////////////////////////////////
	// STATIC HELPERS
	///////////////////////////////////////////////////////////////////////////

	static Move getClosestMove(
		const GameState& game,
		int currentPos,
		std::pair<int,int> target
	);

	static Move getFarthestMove(
		const GameState& game,
		int currentPos,
		std::pair<int,int> target
	);
};

#endif /* MSPACMANCONTROLLER_H_ */

