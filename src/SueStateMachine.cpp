#include "SueStateMachine.h"
#include <iostream>

/////////////////////////////// SueTimerTransition ////////////////////////////////

SueTimerTransition::SueTimerTransition(
	std::shared_ptr<FSMState> next,
	int l
):
	counter(0),
	limit(l),
	_next(next){

}

bool SueTimerTransition::isValid(const GameState&){

	counter++;

	if(counter >= limit){

		counter = 0;

		return true;
	}

	return false;
}

std::shared_ptr<FSMState>
SueTimerTransition::getNextState(){

	return _next;
}

/////////////////////////// SuePowerPelletTransition /////////////////////////////

SuePowerPelletTransition::SuePowerPelletTransition(
	std::shared_ptr<Character> c,
	std::shared_ptr<FSMState> next
):
	_next(next),
	character(c){

}


bool SuePowerPelletTransition::isValid(
	const GameState&
){

	auto ghost =
		std::dynamic_pointer_cast<Ghost>(
			character
		);

	if(
		ghost != nullptr &&
		ghost->isEdible()
	){
		return true;
	}

	return false;
}

std::shared_ptr<FSMState>
SuePowerPelletTransition::getNextState(){

	return _next;
}

///////////////////////// SueFrightenedEndTransition /////////////////////////////

SueFrightenedEndTransition::SueFrightenedEndTransition(
	std::shared_ptr<FSMState> next,
	int l
):
	counter(0),
	limit(l),
	_next(next){

}

bool SueFrightenedEndTransition::isValid(
	const GameState&
){

	counter++;

	if(counter >= limit){

		counter = 0;

		return true;
	}

	return false;
}

std::shared_ptr<FSMState>
SueFrightenedEndTransition::getNextState(){

	return _next;
}

/////////////////////////////// SueChaseState /////////////////////////////////

SueChaseState::SueChaseState(
	std::shared_ptr<Character> _character
):
	FSMState(_character){

}

void SueChaseState::onEnter(const GameState&){

	std::dynamic_pointer_cast<Ghost>(
		character
	)->revert();
}

Move SueChaseState::onUpdate(
	const GameState& game
){

	std::vector<Move> moves;

	const auto pacmanCoord =
		game.getMaze().getNodePos(
			game.getPacmanPos()
		);

	const auto myPos = character->getPos();

	const auto myCoord =
		game.getMaze().getNodePos(
			myPos
		);

	if(character->getDirection() == PASS){

		moves =
			game.getMaze().getPossibleMoves(
				myPos
			);
	}
	else{

		moves =
			game.getMaze().getGhostLegalMoves(
				myPos,
				character->getDirection()
			);
	}

	if(moves.empty()){
		return PASS;
	}

	//////////////////// comportamiento especial ////////////////////

	std::pair<int,int> target;

	float dist =
		euclid2(
			myCoord,
			pacmanCoord
		);

	// si está cerca, escapar
	if(dist < 64.0f){
        	target =
		game.getMaze().getPowerPillPositions()[0];
	}
	else{

		// perseguir
		target = pacmanCoord;
	}

	float min =
		euclid2(
			game.getMaze().getNodePos(
				game.getMaze().getNeighbour(
					myPos,
					moves[0]
				)
			),
			target
		);

	int minI = 0;

	for(unsigned int i=1;i<moves.size();i++){

		auto distMove =
			euclid2(
				game.getMaze().getNodePos(
					game.getMaze().getNeighbour(
						myPos,
						moves[i]
					)
				),
				target
			);

		if(distMove < min){

			min = distMove;

			minI = i;
		}
	}

	return moves[minI];
}

SueChaseState::~SueChaseState(){

}

/////////////////////////////// SueScatterState //////////////////////////////////

SueScatterState::SueScatterState(
	std::shared_ptr<Character> _character
):
	FSMState(_character){

}

void SueScatterState::onEnter(const GameState&){

	std::dynamic_pointer_cast<Ghost>(
		character
	)->revert();
}

Move SueScatterState::onUpdate(
	const GameState& game
){

	std::vector<Move> moves;

	const auto myPos = character->getPos();

	if(character->getDirection() == PASS){

		moves =
			game.getMaze().getPossibleMoves(
				myPos
			);
	}
	else{

		moves =
			game.getMaze().getGhostLegalMoves(
				myPos,
				character->getDirection()
			);
	}

	if(moves.empty()){
		return PASS;
	}

	///////////////// esquina inferior izquierda //////////////////

	const auto targetCoord =
	game.getMaze().getPowerPillPositions()[0];

	float min =
		euclid2(
			game.getMaze().getNodePos(
				game.getMaze().getNeighbour(
					myPos,
					moves[0]
				)
			),
			targetCoord
		);

	int minI = 0;

	for(unsigned int i=1;i<moves.size();i++){

		auto dist =
			euclid2(
				game.getMaze().getNodePos(
					game.getMaze().getNeighbour(
						myPos,
						moves[i]
					)
				),
				targetCoord
			);

		if(dist < min){

			min = dist;

			minI = i;
		}
	}

	return moves[minI];
}

SueScatterState::~SueScatterState(){

}

//////////////////////////// SueFrightenedState /////////////////////////////////

SueFrightenedState::SueFrightenedState(
	std::shared_ptr<Character> _character
):
	FSMState(_character){

}

void SueFrightenedState::onEnter(const GameState&){

	std::dynamic_pointer_cast<Ghost>(
		character
	)->revert();
}

Move SueFrightenedState::onUpdate(
	const GameState& game
){

	std::vector<Move> moves;

	const auto myPos = character->getPos();

	if(character->getDirection() == PASS){

		moves =
			game.getMaze().getPossibleMoves(
				myPos
			);
	}
	else{

		moves =
			game.getMaze().getGhostLegalMoves(
				myPos,
				character->getDirection()
			);
	}

	//

	if(moves.empty()){
		return PASS;
	}


	int r = rand() % moves.size();

	return moves[r];
}

SueFrightenedState::~SueFrightenedState(){

}

/////////////////////////// SueNonFrightenedState ////////////////////////////////

SueNonFrightenedState::SueNonFrightenedState(
	std::shared_ptr<Character> _character
):
	FSMState(_character)
{
	chaseState =
		std::make_shared<SueChaseState>(
			character
		);

	scatterState =
		std::make_shared<SueScatterState>(
			character
		);

	chaseState->addTransition(
		std::make_shared<SueTimerTransition>(
			scatterState,
			300
		)
	);

	scatterState->addTransition(
		std::make_shared<SueTimerTransition>(
			chaseState,
			120
		)
	);

	activeState = chaseState;
}

void SueNonFrightenedState::onEnter(
	const GameState& gs
){
	activeState->onEnter(gs);
}

void SueNonFrightenedState::onExit(
	const GameState& gs
){
	activeState->onExit(gs);
}

Move SueNonFrightenedState::onUpdate(
	const GameState& gs
){

	auto t =
		activeState->getActiveTransition(gs);

	if(t != nullptr){

		activeState->onExit(gs);

		t->onTransition(gs);

		activeState =
			t->getNextState();

		activeState->onEnter(gs);
	}

	return activeState->onUpdate(gs);
}

SueNonFrightenedState::~SueNonFrightenedState(){

}

/////////////////////////// SueStateMachine ////////////////////////////////

SueStateMachine::SueStateMachine(
	std::shared_ptr<Character> _character
):
	FiniteStateMachine(_character)
{
	auto nonFrightened =
		std::make_shared<SueNonFrightenedState>(
			character
		);

	auto frightened =
		std::make_shared<SueFrightenedState>(
			character
		);

	nonFrightened->addTransition(
		std::make_shared<SuePowerPelletTransition>(
			character,
			frightened
		)
	);

	frightened->addTransition(
		std::make_shared<SueFrightenedEndTransition>(
			nonFrightened,
			300
		)
	);

	initialState = nonFrightened;

	activeState = initialState;

	states.push_back(nonFrightened);
	states.push_back(frightened);
}

Move SueStateMachine::update(
	const GameState& gs
){

	auto t =
		activeState->getActiveTransition(gs);

	if(t != nullptr){

		activeState->onExit(gs);

		t->onTransition(gs);

		activeState =
			t->getNextState();

		activeState->onEnter(gs);
	}

	return activeState->onUpdate(gs);
}

SueStateMachine::~SueStateMachine(){

}