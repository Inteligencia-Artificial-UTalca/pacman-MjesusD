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
		std::make_shared<PinkyChase>()
	);
}

/////////////////////////// Comportamiento Pinky //////////////////////////////

Status PinkyChase::update(){

	auto character =
		Info::getInfo()->in_character;

	auto gs =
		Info::getInfo()->in_gamestate;

	//////////////// posición objetivo ////////////////////

	auto target =
		gs->getMaze().getNodePos(
			gs->getPacmanPos()
		);

	/////////////// Pinky apunta delante de Pacman /////////

	switch(gs->getPacmanDir()){

		case UP:
			target.second -= 64;
			break;

		case DOWN:
			target.second += 64;
			break;

		case LEFT:
			target.first -= 64;
			break;

		case RIGHT:
			target.first += 64;
			break;

		default:
			break;
	}

	///////////////////////////////////////////////////////

	float min = 1000000000;

	Move minMove = PASS;

	std::vector<Move> moves;

	if(character->getDirection() == PASS){

		moves =
			gs->getMaze().getPossibleMoves(
				character->getPos()
			);
	}
	else{

		moves =
			gs->getMaze().getGhostLegalMoves(
				character->getPos(),
				character->getDirection()
			);
	}

	for(auto move : moves){

		if(move == PASS){
			break;
		}

		auto nextPos =
			gs->getMaze().getNodePos(
				gs->getMaze().getNeighbour(
					character->getPos(),
					move
				)
			);

		float dist =
			euclid2(
				target,
				nextPos
			);

		if(dist < min){

			min = dist;

			minMove = move;
		}
	}

	Info::getInfo()->out_move =
		minMove;

	return BH_SUCCESS;
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


