#include "PacmanController.h"

#include <SDL2/SDL.h>

#include <iostream>
#include <limits>
#include <chrono>

PacmanController::PacmanController(
	std::shared_ptr<Character> character
):
	Controller(character),
	root(std::make_shared<Selector>())
{

//////////////////////////////// ESCAPE //////////////////////////////////

	auto escapeFilter =
		std::make_shared<Filter>();

	escapeFilter->addCondition(
		std::make_shared<GhostNearby>()
	);

	escapeFilter->addAction(
		std::make_shared<Escape>()
	);

	root->addChild(escapeFilter);

	//////////////////////////////// SEEK PILL //////////////////////////////////

	root->addChild(
		std::make_shared<SeekPill>()
	);

	//////////////////////////////// RANDOM //////////////////////////////////

	root->addChild(
		std::make_shared<RandomMoveAction>()
	);
}

PacmanController::~PacmanController(){

}

Move PacmanController::getMove(
	const GameState& game
){

//////////////////////////////// SDL //////////////////////////////////

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

	//////////////////////////////// BT INFO //////////////////////////////////

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
		<< "Pacman BT Time: "
		<< diff.count()
		<< " seconds\n";

	return Info::getInfo()->out_move;
}

//////////////////////////////// GHOSTS //////////////////////////////////

Status GhostNearby::update(){

	auto gs =
		Info::getInfo()->in_gamestate;

	auto character =
		Info::getInfo()->in_character;

	auto myCoord =
		gs->getMaze().getNodePos(
			character->getPos()
		);

	for(auto ghost : gs->getGhosts()){

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

//////////////////////////////// ESCAPE //////////////////////////////////

Status Escape::update(){

	auto gs =
		Info::getInfo()->in_gamestate;

	auto character =
		Info::getInfo()->in_character;

	std::vector<Move> moves;

	if(character->getDirection() == PASS){

		moves =
			gs->getMaze().getPossibleMoves(
				character->getPos()
			);
	}
	else{

		moves =
			gs->getMaze().getPossibleMoves(
				character->getPos()
			);
	}

	float maxDist = -1.0f;

	Move bestMove = PASS;

	for(auto move : moves){

		auto nextCoord =
			gs->getMaze().getNodePos(
				gs->getMaze().getNeighbour(
					character->getPos(),
					move
				)
			);

		float nearestGhost = 0.0f;

		for(auto ghost : gs->getGhosts()){

			auto ghostCoord =
				gs->getMaze().getNodePos(
					ghost->getPos()
				);

			float dist =
				euclid2(
					nextCoord,
					ghostCoord
				);

			if(dist > nearestGhost){

				nearestGhost = dist;
			}
		}

		if(nearestGhost > maxDist){

			maxDist = nearestGhost;

			bestMove = move;
		}
	}

	Info::getInfo()->out_move =
		bestMove;

	return BH_SUCCESS;
}

//////////////////////////////// SEEK PILL //////////////////////////////////

Status SeekPill::update(){

	auto gs =
		Info::getInfo()->in_gamestate;

	auto character =
		Info::getInfo()->in_character;

	auto pills =
		gs->getMaze().getPillPositions();

	if(pills.empty()){

		return BH_FAILURE;
	}

	auto target = pills[0];

	std::vector<Move> moves =
		gs->getMaze().getPossibleMoves(
			character->getPos()
		);

	float min =
		std::numeric_limits<float>::max();

	Move bestMove = PASS;

	for(auto move : moves){

		auto nextCoord =
			gs->getMaze().getNodePos(
				gs->getMaze().getNeighbour(
					character->getPos(),
					move
				)
			);

		float dist =
			euclid2(
				target,
				nextCoord
			);

		if(dist < min){

			min = dist;

			bestMove = move;
		}
	}

	Info::getInfo()->out_move =
		bestMove;

	return BH_SUCCESS;
}

//////////////////////////////// RANDOM //////////////////////////////////

Status RandomMoveAction::update(){

	auto gs =
		Info::getInfo()->in_gamestate;

	auto character =
		Info::getInfo()->in_character;

	auto moves =
		gs->getMaze().getPossibleMoves(
			character->getPos()
		);

	if(moves.empty()){

		Info::getInfo()->out_move =
			PASS;
	}
	else{

		Info::getInfo()->out_move =
			moves[rand() % moves.size()];
	}

	return BH_SUCCESS;
}