
#include "MsPacmanController.h"

#include <SDL2/SDL.h>

#include <iostream>
#include <limits>
#include <chrono>

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


	auto objectiveSelector =
		std::make_shared<Selector>();

	//////////////////////// SEEK POWER PILL //////////////////////////////////

	auto powerPillFilter =
		std::make_shared<Filter>();

	powerPillFilter->addCondition(
		std::make_shared<MsPowerPillAvailable>()
	);

	powerPillFilter->addAction(
		std::make_shared<MsSeekPowerPill>()
	);

	objectiveSelector->addChild(
		powerPillFilter
	);

	//////////////////////////// SEEK PILL ////////////////////////////////////

	objectiveSelector->addChild(
		std::make_shared<MsSeekPill>()
	);

	root->addChild(
		objectiveSelector
	);


//////////////////////////// LOW PRIORITY /////////////////////////////////////


	root->addChild(
		std::make_shared<MsRandomMoveAction>()
	);
}

MsPacmanController::~MsPacmanController(){

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

	auto start =
		std::chrono::high_resolution_clock::now();

	root->tick();

	auto end =
		std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> diff =
		end - start;

	std::cout
		<< "Ms Pacman BT Time: "
		<< diff.count()
		<< " seconds\n";

	return Info::getInfo()->out_move;
}


/////////////////////////// GHOST NEARBY //////////////////////////////////////


Status MsGhostNearby::update(){

	auto gs =
		Info::getInfo()->in_gamestate;

	auto character =
		Info::getInfo()->in_character;

	auto myCoord =
		gs->getMaze().getNodePos(
			character->getPos()
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
			euclid2(
				myCoord,
				ghostCoord
			);

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

	auto character =
		Info::getInfo()->in_character;

	auto myCoord =
		gs->getMaze().getNodePos(
			character->getPos()
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
			euclid2(
				myCoord,
				ghostCoord
			);

		if(dist < 10.0f){

			return BH_SUCCESS;
		}
	}

	return BH_FAILURE;
}


//////////////////////// POWER PILL AVAILABLE /////////////////////////////////


Status MsPowerPillAvailable::update(){

	auto gs =
		Info::getInfo()->in_gamestate;

	auto character =
		Info::getInfo()->in_character;

	auto powers =
		gs->getMaze().getPowerPillPositions();

	if(powers.empty()){

		return BH_FAILURE;
	}

	auto myCoord =
		gs->getMaze().getNodePos(
			character->getPos()
		);

	// buscalibre un fantasma peligroso está cerca
	for(auto ghost : gs->getGhosts()){

		if(ghost->isEdible()){
			continue;
		}

		auto ghostCoord =
			gs->getMaze().getNodePos(
				ghost->getPos()
			);

		float dist =
			euclid2(
				myCoord,
				ghostCoord
			);

		if(dist < 6.0f){

			return BH_SUCCESS;
		}
	}

	return BH_FAILURE;
}


//////////////////////////////// ESCAPE ///////////////////////////////////////


Status MsEscape::update(){

	auto gs =
		Info::getInfo()->in_gamestate;

	auto character =
		Info::getInfo()->in_character;

	auto moves =
		gs->getMaze().getPossibleMoves(
			character->getPos()
		);

	float bestScore = -1.0f;

	Move bestMove = PASS;

	for(auto move : moves){

		int neighbour =
			gs->getMaze().getNeighbour(
				character->getPos(),
				move
			);

		if(neighbour == -1){
			continue;
		}

		auto nextCoord =
			gs->getMaze().getNodePos(
				neighbour
			);

		float nearestGhost = std::numeric_limits<float>::max();

		for(auto ghost : gs->getGhosts()){

			if(ghost->isEdible()){
				continue;
			}

			auto ghostCoord =
				gs->getMaze().getNodePos(
					ghost->getPos()
				);

			float dist =
				euclid2(
					nextCoord,
					ghostCoord
				);

			if(dist < nearestGhost){

				nearestGhost = dist;
			}
		}

		if(nearestGhost > bestScore){

			bestScore = nearestGhost;

			bestMove = move;
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

	auto character =
		Info::getInfo()->in_character;

	auto moves =
		gs->getMaze().getPossibleMoves(
			character->getPos()
		);

	if(moves.empty()){

		return BH_FAILURE;
	}

	float bestDist =
		std::numeric_limits<float>::max();

	Move bestMove = PASS;

	for(auto move : moves){

		int neighbour =
			gs->getMaze().getNeighbour(
				character->getPos(),
				move
			);

		if(neighbour == -1){
			continue;
		}

		auto nextCoord =
			gs->getMaze().getNodePos(
				neighbour
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
				euclid2(
					nextCoord,
					ghostCoord
				);

			// evitar perseguir fantasmas demasiado lejos
			if(dist > 10.0f){
				continue;
			}

			if(dist < bestDist){

				bestDist = dist;
				bestMove = move;
			}
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

	auto character =
		Info::getInfo()->in_character;

	auto powers =
		gs->getMaze().getPowerPillPositions();

	if(powers.empty()){

		return BH_FAILURE;
	}

	auto myCoord =
		gs->getMaze().getNodePos(
			character->getPos()
		);

	auto moves =
		gs->getMaze().getPossibleMoves(
			character->getPos()
		);

	if(moves.empty()){

		return BH_FAILURE;
	}

	float bestGlobalDist =
		std::numeric_limits<float>::max();

	Move bestMove = PASS;

	for(auto move : moves){

		int neighbour =
			gs->getMaze().getNeighbour(
				character->getPos(),
				move
			);

		if(neighbour == -1){
			continue;
		}

		auto nextCoord =
			gs->getMaze().getNodePos(
				neighbour
			);

		for(auto powerCoord : powers){

			float dist =
				euclid2(
					nextCoord,
					powerCoord
				);

			if(dist < bestGlobalDist){

				bestGlobalDist = dist;
				bestMove = move;
			}
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

	auto character =
		Info::getInfo()->in_character;

	auto pills =
		gs->getMaze().getPillPositions();

	if(pills.empty()){

		return BH_FAILURE;
	}

	auto moves =
		gs->getMaze().getPossibleMoves(
			character->getPos()
		);

	float bestDist =
		std::numeric_limits<float>::max();

	Move bestMove = PASS;

	for(auto move : moves){

		int neighbour =
			gs->getMaze().getNeighbour(
				character->getPos(),
				move
			);

		if(neighbour == -1){
			continue;
		}

		auto nextCoord =
			gs->getMaze().getNodePos(
				neighbour
			);

		for(auto pillCoord : pills){

			float dist =
				euclid2(
					pillCoord,
					nextCoord
				);

			if(dist < bestDist){

				bestDist = dist;

				bestMove = move;
			}
		}
	}

	if(bestMove == PASS){

	  return BH_FAILURE;

    }

    Info::getInfo()->out_move = bestMove;

    return BH_SUCCESS;
}


//////////////////////////// RANDOM ///////////////////////////////////////////


Status MsRandomMoveAction::update(){

	auto gs =
		Info::getInfo()->in_gamestate;

	auto character =
		Info::getInfo()->in_character;

	auto moves =
		gs->getMaze().getPossibleMoves(
			character->getPos()
		);

	if(moves.empty()){

		return BH_FAILURE;
	}

	Info::getInfo()->out_move =
		moves[rand() % moves.size()];

	return BH_SUCCESS;
}