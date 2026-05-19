#include "InkyController.h"
#include "BTGhostController.h"
#include "BehaviorTree.h"
#include "Ghost.h"

#include <iostream>
#include <chrono>

InkyController::InkyController(
	std::shared_ptr<Character> character
):
	Controller(character),
	root(std::make_shared<Selector>())
{

	//////////////////// FrightenedState ////////////////////

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

	//////////////////// InkyChaseState /////////////////////

	root->addChild(
		std::make_shared<InkyChase>()
	);
}

/////////////////////////// Comportamiento Inky ////////////////////////////////

Status InkyChase::update(){

	auto character =
		Info::getInfo()->in_character;

	auto blinky =
		Info::getInfo()->blinky;

	auto gs =
		Info::getInfo()->in_gamestate;

	auto pacmanPos =
		gs->getMaze().getNodePos(
			gs->getPacmanPos()
		);

	auto blinkyPos =
		gs->getMaze().getNodePos(
			blinky->getPos()
		);

	//////////////// punto delante de Pacman ////////////////

	std::pair<int,int> frontPacman =
		pacmanPos;

	switch(gs->getPacmanDir()){

		case UP:
			frontPacman.second -= 32;
			break;

		case DOWN:
			frontPacman.second += 32;
			break;

		case LEFT:
			frontPacman.first -= 32;
			break;

		case RIGHT:
			frontPacman.first += 32;
			break;

		default:
			break;
	}

	//////////////// target relativo a Blinky ///////////////

	std::pair<int,int> target;

	target.first =
		frontPacman.first +
		(frontPacman.first - blinkyPos.first);

	target.second =
		frontPacman.second +
		(frontPacman.second - blinkyPos.second);

	/////////////////////////////////////////////////////////

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

		float dist =
			euclid2(
				target,
				gs->getMaze().getNodePos(
					gs->getMaze().getNeighbour(
						character->getPos(),
						move
					)
				)
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


InkyController::~InkyController(){

}

Move InkyController::getMove(
	const GameState& game
){

	Info::getInfo()->in_character =
		character;

	Info::getInfo()->in_gamestate =
		&game;

	root->tick();

	return Info::getInfo()->out_move;
}

