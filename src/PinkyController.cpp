#include "PinkyController.h"
#include "BTGhostController.h"
#include "BehaviorTree.h"
#include "Ghost.h"

#include <iostream>
#include <chrono>

PinkyController::PinkyController(
	std::shared_ptr<Character> character
):
	Controller(character),
	root(std::make_shared<Selector>())
{

	//////////////////// FrightnedState ////////////////////

	auto frightenedFilter =
		std::make_shared<Filter>();

	frightenedFilter->addCondition(
		std::make_shared<Powerpill>()
	);

	frightenedFilter->addAction(
		std::make_shared<Frightened>()
	);

	root->addChild(frightenedFilter);

	//////////////////// ScatterState ///////////////////////

	auto scatterFilter =
		std::make_shared<Filter>();

	scatterFilter->addCondition(
		std::make_shared<TimeOut>()
	);

	scatterFilter->addAction(
		std::make_shared<Scatter>()
	);

	root->addChild(scatterFilter);

	//////////////////// ChaseState /////////////////////////

	root->addChild(
		std::make_shared<Chase>()
	);
}

PinkyController::~PinkyController(){

}

Move PinkyController::getMove(
	const GameState& game
){

	Info::getInfo()->in_character =
		character;

	Info::getInfo()->in_gamestate =
		&game;

	root->tick();

	return Info::getInfo()->out_move;
}