#include "BlinkyStateMachine.h"
#include <iostream>

/////////////////////////////// TimerTransition ////////////////////////////////

TimerTransition::TimerTransition(
	std::shared_ptr<FSMState> next,
	int l
):
	counter(0),
	limit(l),
	_next(next){

}

bool TimerTransition::isValid(const GameState&){

	counter++;

	if(counter >= limit){

		counter = 0;

		return true;
	}

	return false;
}

std::shared_ptr<FSMState>
TimerTransition::getNextState(){

	return _next;
}

/////////////////////////// PowerPelletTransition /////////////////////////////

PowerPelletTransition::PowerPelletTransition(
	std::shared_ptr<Character> c,
	std::shared_ptr<FSMState> next
):
	_next(next),
	character(c){

}


bool PowerPelletTransition::isValid(
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
PowerPelletTransition::getNextState(){

	return _next;
}

///////////////////////// FrightenedEndTransition /////////////////////////////

FrightenedEndTransition::FrightenedEndTransition(
	std::shared_ptr<FSMState> next,
	int l
):
	counter(0),
	limit(l),
	_next(next){

}

bool FrightenedEndTransition::isValid(
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
FrightenedEndTransition::getNextState(){

	return _next;
}

//////////////////////////////// ChaseState ///////////////////////////////////

ChaseState::ChaseState(
	std::shared_ptr<Character> _character
):
	FSMState(_character), firstEnter(true){

}

void ChaseState::onEnter(const GameState&){

	if(firstEnter){

		firstEnter = false;

		return;
	}

	std::dynamic_pointer_cast<Ghost>(
		character
	)->revert();
}

Move ChaseState::onUpdate(
	const GameState& game
){

	std::vector<Move> moves;

	const auto pacmanCoord =
		game.getMaze().getNodePos(
			game.getPacmanPos()
		);

	const auto myPos = character->getPos();

	int pillsLeft = game.getMaze().getPillPositions().size();

	bool cruiseElroy = false;

	// mientras menos pills queden, blinky entra en modo agresivo

	if(pillsLeft < 10){
		cruiseElroy = true;
	}
	
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

	float bestValue =
		euclid2(
			game.getMaze().getNodePos(
				game.getMaze().getNeighbour(
					myPos,
					moves[0]
				)
			),
			pacmanCoord
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
			pacmanCoord
		);

	if(cruiseElroy){

		// hace a Blinky más directo/agresivo
		dist *= 0.5f;
	}

	if(dist < bestValue){

		bestValue = dist;

		minI = i;
	}
}

	return moves[minI];
}

ChaseState::~ChaseState(){

}

/////////////////////////////// ScatterState //////////////////////////////////

ScatterState::ScatterState(
	std::shared_ptr<Character> _character
):
	FSMState(_character), firstEnter(true){

}

void ScatterState::onEnter(const GameState&){

	if(firstEnter){

		firstEnter = false;

		return;
	}

	std::dynamic_pointer_cast<Ghost>(
		character
	)->revert();
}

Move ScatterState::onUpdate(
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

	// Corner seguro temporal
	int targetCorner = 0;

	const auto targetCoord =
		game.getMaze().getNodePos(
			targetCorner
		);

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

ScatterState::~ScatterState(){

}

//////////////////////////// FrightenedState /////////////////////////////////

FrightenedState::FrightenedState(
	std::shared_ptr<Character> _character
):
	FSMState(_character), firstEnter(true){

}

void FrightenedState::onEnter(const GameState&){

	if(firstEnter){

		firstEnter = false;

		return;
	}

	std::dynamic_pointer_cast<Ghost>(
		character
	)->revert();
}

Move FrightenedState::onUpdate(
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

FrightenedState::~FrightenedState(){

}

/////////////////////////// NonFrightenedState ////////////////////////////////

NonFrightenedState::NonFrightenedState(
	std::shared_ptr<Character> _character
):
	FSMState(_character)
{
	chaseState =
		std::make_shared<ChaseState>(
			character
		);

	scatterState =
		std::make_shared<ScatterState>(
			character
		);

	chaseState->addTransition(
		std::make_shared<TimerTransition>(
			scatterState,
			300
		)
	);

	scatterState->addTransition(
		std::make_shared<TimerTransition>(
			chaseState,
			120
		)
	);

	activeState = chaseState;
}

void NonFrightenedState::onEnter(
	const GameState& gs
){
	activeState->onEnter(gs);
}

void NonFrightenedState::onExit(
	const GameState& gs
){
	activeState->onExit(gs);
}

Move NonFrightenedState::onUpdate(
	const GameState& gs
){

	int pillsLeft =
		gs.getMaze().getPillPositions().size();

	bool cruiseElroy =
		(pillsLeft < 10);

	if(!cruiseElroy){

		auto t =
			activeState->getActiveTransition(gs);

		if(t != nullptr){

			activeState->onExit(gs);

			t->onTransition(gs);

			activeState =
				t->getNextState();

			activeState->onEnter(gs);
		}
	}

	return activeState->onUpdate(gs);
}

NonFrightenedState::~NonFrightenedState(){

}

/////////////////////////// BlinkyStateMachine ////////////////////////////////

BlinkyStateMachine::BlinkyStateMachine(
	std::shared_ptr<Character> _character
):
	FiniteStateMachine(_character)
{
	auto nonFrightened =
		std::make_shared<NonFrightenedState>(
			character
		);

	auto frightened =
		std::make_shared<FrightenedState>(
			character
		);

	nonFrightened->addTransition(
		std::make_shared<PowerPelletTransition>(
			character,
			frightened
		)
	);

	frightened->addTransition(
		std::make_shared<FrightenedEndTransition>(
			nonFrightened,
			300
		)
	);

	initialState = nonFrightened;

	activeState = initialState;

	states.push_back(nonFrightened);
	states.push_back(frightened);
}

Move BlinkyStateMachine::update(
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

BlinkyStateMachine::~BlinkyStateMachine(){

}