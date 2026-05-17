#include "SueController.h"

SueController::SueController(
	std::shared_ptr<Character> character
):
	Controller(character),
	fsm(std::make_shared<SueStateMachine>(
		character
	))
{

}

SueController::~SueController(){

}

Move SueController::getMove(
	const GameState& game
){
	return fsm->update(game);
}