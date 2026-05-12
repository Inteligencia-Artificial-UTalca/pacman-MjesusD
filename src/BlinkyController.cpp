#include "BlinkyController.h"

BlinkyController::BlinkyController(
	std::shared_ptr<Character> character
):
	Controller(character),
	fsm(std::make_shared<BlinkyStateMachine>(
		character
	))
{

}

BlinkyController::~BlinkyController(){

}

Move BlinkyController::getMove(
	const GameState& game
){
	return fsm->update(game);
}