#include "MsPacmanController.h"

#include <SDL2/SDL.h>

#include <iostream>
#include <limits>
#include <chrono>
#include <cmath>

////////////////////////// CONTROLLER /////////////////////////////////////////

MsPacmanController::MsPacmanController(
	std::shared_ptr<Character> character
):
	Controller(character),
	root(std::make_shared<Selector>())
{

/////////////////////////// HIGH PRIORITY /////////////////////////////////////

	auto dangerSelector =
		std::make_shared<Selector>();

///////////////////////// EAT EDIBLE GHOST /////////////////////////////////

	auto eatGhostFilter =
		std::make_shared<Filter>();

	eatGhostFilter->addCondition(
		std::make_shared<MsEdibleGhostNearby>()
	);

	eatGhostFilter->addAction(
		std::make_shared<MsEatGhost>()
	);

	dangerSelector->addChild(
		eatGhostFilter
	);


//////////////////////// SEEK POWER PILL //////////////////////////////////

	auto powerPillFilter =
		std::make_shared<Filter>();

	powerPillFilter->addCondition(
		std::make_shared<MsPowerPillAvailable>()
	);

	powerPillFilter->addAction(
		std::make_shared<MsSeekPowerPill>()
	);

	dangerSelector->addChild(
		powerPillFilter
	);

//////////////////////////// ESCAPE ///////////////////////////////////////

	auto escapeFilter =
		std::make_shared<Filter>();

	escapeFilter->addCondition(
		std::make_shared<MsGhostNearby>()
	);

	escapeFilter->addAction(
		std::make_shared<MsEscape>()
	);

	dangerSelector->addChild(
		escapeFilter
	);

	root->addChild(
		dangerSelector
	);


//////////////////////////// MID PRIORITY /////////////////////////////////////

	auto objectiveSelector = std::make_shared<Selector>();

	

//////////////////////////// SEEK PILL ////////////////////////////////////

	objectiveSelector->addChild( std::make_shared<MsSeekPill>() ); 
	root->addChild( objectiveSelector );

//////////////////////////// LOW PRIORITY /////////////////////////////////////

	root->addChild(
		std::make_shared<MsRandomMoveAction>()
	);
}

MsPacmanController::~MsPacmanController(){

}

////////////////////////// CLOSEST MOVE ///////////////////////////////////////

Move MsPacmanController::getClosestMove(
	const GameState& game,
	int currentPos,
	std::pair<int,int> target
){

	int minDist = 99999999;

	Move bestMove = PASS;

	auto moves =
		game.getMaze().getPossibleMoves(
			currentPos
		);

	for(auto move : moves){

		int neighbour =
			game.getMaze().getNeighbour(
				currentPos,
				move
			);

		if(neighbour < 0){
			continue;
		}

		auto nextCoord =
			game.getMaze().getNodePos(
				neighbour
			);

		int dist =
			euclid2(nextCoord,target);

		if(dist < minDist){

			minDist = dist;
			bestMove = move;
		}
	}

	if(bestMove == PASS && !moves.empty()){ 
		bestMove = moves[0]; 
	}

	return bestMove;
}

////////////////////////// FARTHEST MOVE //////////////////////////////////////

Move MsPacmanController::getFarthestMove(
	const GameState& game,
	int currentPos,
	std::pair<int,int> target
){

	int maxDist = -1;

	Move bestMove = PASS;

	auto moves =
		game.getMaze().getPossibleMoves(
			currentPos
		);

	for(auto move : moves){

		int neighbour =
			game.getMaze().getNeighbour(
				currentPos,
				move
			);

		if(neighbour < 0){
			continue;
		}

		auto nextCoord =
			game.getMaze().getNodePos(
				neighbour
			);

		int dist =
			euclid2(nextCoord,target);

		if(dist > maxDist){

			maxDist = dist;
			bestMove = move;
		}
	}

	if(bestMove == PASS && !moves.empty()){ 
		bestMove = moves[0]; 
	}
	
	return bestMove;
}

//////////////////////////////// GET MOVE /////////////////////////////////////

Move MsPacmanController::getMove(
	const GameState& game
){

	SDL_Event e;

	if(SDL_PollEvent(&e) != 0){

		if(
			e.type == SDL_QUIT ||

			(
				e.type == SDL_KEYDOWN &&

				(
					e.key.keysym.sym == SDLK_ESCAPE ||
					e.key.keysym.sym == SDLK_q
				)
			)
		){
			SDL_Quit();
			exit(0);
		}
	}

	Info::getInfo()->in_character =
		character;

	Info::getInfo()->in_gamestate =
		&game;

	
	Status result = root->tick();

	if(result == BH_FAILURE){

	auto moves =
		game.getMaze().getPossibleMoves(
			character->getPos()
		);

		if(!moves.empty()){

			Info::getInfo()->out_move =
				moves[0];
		}
	}

	return Info::getInfo()->out_move;
}

/////////////////////////// GHOST NEARBY //////////////////////////////////////

Status MsGhostNearby::update(){

	auto gs =
		Info::getInfo()->in_gamestate;

	auto myCoord =
		gs->getMaze().getNodePos(
			Info::getInfo()->in_character->getPos()
		);

	for(auto ghost : gs->getGhosts()){

		if(ghost->isEdible()){
			continue;
		}

		auto ghostCoord =
			gs->getMaze().getNodePos(
				ghost->getPos()
			);

		float dist =
			sqrt(euclid2(myCoord,ghostCoord));

		if(dist < 5.0f){

			return BH_SUCCESS;
		}
	}

	return BH_FAILURE;
}

//////////////////////// EDIBLE GHOST NEARBY //////////////////////////////////

Status MsEdibleGhostNearby::update(){

	auto gs =
		Info::getInfo()->in_gamestate;

	auto myCoord =
		gs->getMaze().getNodePos(
			Info::getInfo()->in_character->getPos()
		);

	for(auto ghost : gs->getGhosts()){

		if(!ghost->isEdible()){
			continue;
		}

		auto ghostCoord =
			gs->getMaze().getNodePos(
				ghost->getPos()
			);

		float dist =
			sqrt(euclid2(myCoord,ghostCoord));

		if(dist < 7.0f){

			return BH_SUCCESS;
		}
	}

	return BH_FAILURE;
}

//////////////////////// POWER PILL AVAILABLE /////////////////////////////////

Status MsPowerPillAvailable::update(){

	auto gs =
		Info::getInfo()->in_gamestate;

	auto powers =
		gs->getMaze().getPowerPillPositions();

	if(powers.empty()){

		return BH_FAILURE;
	}

	auto myCoord =
		gs->getMaze().getNodePos(
			Info::getInfo()->in_character->getPos()
		);

	for(auto ghost : gs->getGhosts()){

		if(ghost->isEdible()){
			continue;
		}

		auto ghostCoord =
			gs->getMaze().getNodePos(
				ghost->getPos()
			);

		float dist =
			sqrt(euclid2(myCoord,ghostCoord));

		if(dist < 20.0f){

			return BH_SUCCESS;
		}
	}

	return BH_FAILURE;
}

//////////////////////////////// ESCAPE ///////////////////////////////////////

Status MsEscape::update(){

	auto gs =
		Info::getInfo()->in_gamestate;

	int currentPos =
		Info::getInfo()->in_character->getPos();

	auto myCoord =
		gs->getMaze().getNodePos(
			currentPos
		);

	float danger = -1.0f;

	Move bestMove = PASS;

	for(auto ghost : gs->getGhosts()){

		if(ghost->isEdible()){
			continue;
		}

		auto ghostCoord =
			gs->getMaze().getNodePos(
				ghost->getPos()
			);

		float dist =
			sqrt(euclid2(myCoord,ghostCoord));

		float fear =
			1.0f -
			1.0f /
			(1.0f + pow(2.718f * 0.45f,-dist + 32.0f));

		if(fear > danger){

			danger = fear;

			bestMove =
				MsPacmanController::getFarthestMove(
					*gs,
					currentPos,
					ghostCoord
				);
		}
	}

	if(bestMove == PASS){
		return BH_FAILURE;
	}

	Info::getInfo()->out_move =
		bestMove;

	return BH_SUCCESS;
}

////////////////////////////// EAT GHOST //////////////////////////////////////

Status MsEatGhost::update(){

	auto gs =
		Info::getInfo()->in_gamestate;

	int currentPos =
		Info::getInfo()->in_character->getPos();

	auto myCoord =
		gs->getMaze().getNodePos(
			currentPos
		);

	float hunger = 0.0f;

	Move bestMove = PASS;

	for(auto ghost : gs->getGhosts()){

		if(!ghost->isEdible()){
			continue;
		}

		auto ghostCoord =
			gs->getMaze().getNodePos(
				ghost->getPos()
			);

		float dist =
			sqrt(euclid2(myCoord,ghostCoord));

		float desire =
			pow(100.0f - dist,2) /
			pow(100.0f,2);

		if(desire > hunger){

			hunger = desire;

			bestMove =
				MsPacmanController::getClosestMove(
					*gs,
					currentPos,
					ghostCoord
				);
		}
	}

	if(bestMove == PASS){
		return BH_FAILURE;
	}

	Info::getInfo()->out_move =
		bestMove;

	return BH_SUCCESS;
}

/////////////////////////// SEEK POWER PILL ///////////////////////////////////

Status MsSeekPowerPill::update(){

	auto gs =
		Info::getInfo()->in_gamestate;

	int currentPos =
		Info::getInfo()->in_character->getPos();

	auto myCoord =
		gs->getMaze().getNodePos(
			currentPos
		);

	auto powers =
		gs->getMaze().getPowerPillPositions();

	if(powers.empty()){
		return BH_FAILURE;
	}

	float bestDist =
		std::numeric_limits<float>::max();

	Move bestMove = PASS;

	for(auto power : powers){

		float dist =
			euclid2(myCoord,power);

		if(dist < bestDist){

			bestDist = dist;

			bestMove =
				MsPacmanController::getClosestMove(
					*gs,
					currentPos,
					power
				);
		}
	}

	if(bestMove == PASS){
		return BH_FAILURE;
	}

	Info::getInfo()->out_move =
		bestMove;

	return BH_SUCCESS;
}

//////////////////////////// SEEK PILL ////////////////////////////////////////

Status MsSeekPill::update(){

	auto gs =
		Info::getInfo()->in_gamestate;

	int currentPos =
		Info::getInfo()->in_character->getPos();

	auto myCoord =
		gs->getMaze().getNodePos(
			currentPos
		);

	auto pills =
		gs->getMaze().getPillPositions();

	if(pills.empty()){
		return BH_FAILURE;
	}

	float bestDist =
		std::numeric_limits<float>::max();

	Move bestMove = PASS;

	for(auto pill : pills){

		float dist =
			euclid2(myCoord,pill);

		if(dist < bestDist){

			bestDist = dist;

			bestMove =
				MsPacmanController::getClosestMove(
					*gs,
					currentPos,
					pill
				);
		}
	}

	if(bestMove == PASS){
		return BH_FAILURE;
	}

	Info::getInfo()->out_move =
		bestMove;

	return BH_SUCCESS;
}

//////////////////////////// RANDOM ///////////////////////////////////////////

Status MsRandomMoveAction::update(){

	auto gs =
		Info::getInfo()->in_gamestate;

	auto moves =
		gs->getMaze().getPossibleMoves(
			Info::getInfo()->in_character->getPos()
		);

	if(moves.empty()){

		return BH_FAILURE;
	}

	Info::getInfo()->out_move =
		moves[rand() % moves.size()];

	return BH_SUCCESS;
}

